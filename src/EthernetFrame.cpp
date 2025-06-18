#include "EthernetFrame.hpp"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <endian.h>
#include <stdexcept>

namespace Ethernet {

Frame::Frame() {
  payload.resize(PAYLOAD_LEN_MIN, 0);
  this->crc = Frame::crc32(*this);
}

Frame::Frame(std::span<const uint8_t> data) {
  // Size checks
  if(FRAME_LEN_MAX < data.size()){
    throw std::runtime_error("Data too long");
  }
  if(data.size() < FRAME_LEN_MIN){
    throw std::runtime_error("Data too short");
  }

  // According to:
  // https://en.wikipedia.org/wiki/Ethernet_frame#Frame_%E2%80%93_data_link_layer
  // The ethernet frame Preamble, SFD, and IPG are handled in the physical
  // layer; therefore, checking will be omitted here.

  // For simplicity, we will use 1 iterator that is constantly moved forward
  auto iter = data.begin();

  // Destination MAC address
  std::copy_n(iter, MAC_LEN, this->dst.begin());
  iter += MAC_LEN;

  // Source MAC address
  std::copy_n(iter, MAC_LEN, this->src.begin());
  iter += MAC_LEN;

  // EtherType
  std::memcpy(&this->type, &(*iter), sizeof(EtherType));
  this->type = static_cast<EtherType>(
      std::byteswap(static_cast<uint16_t>(this->type)));
  iter += sizeof(this->type);

  // Calculate payload size
  const std::size_t payload_len = data.size()
      - 2 * MAC_LEN
      - sizeof(this->type)
      - sizeof(this->crc);

  // There doesn't need to be a size check here since the minimum and maximum
  // are verified at the beginning of this function
  this->payload.resize(payload_len);
  std::copy_n(iter, payload_len, this->payload.begin());
  iter += payload_len;

  // CRC
  // This memcpy will keep the crc in big-endian; however, since we use the
  // reverse CRC polynomial our CRC output will also be big-endian to match
  std::memcpy(&this->crc, &(*iter), sizeof(this->crc));

  // Validate CRC
  if(!this->isValid()){
    throw std::runtime_error("Provided CRC32 does not match calculated CRC32");
  }
}

Frame::Frame(const MacAddr &dst, const MacAddr &src, const EtherType type,
             const std::vector<uint8_t> &data)
    : dst(dst), src(src), type(type) {
  // Check data isn't too long
  if (PAYLOAD_LEN_MAX < data.size()) {
    throw std::runtime_error("Payload too large");
  }
  if (data.size() < PAYLOAD_LEN_MIN){
    throw std::runtime_error("Payload too small");
  }

  // Copy data
  this->payload = data;

  // Generate CRC32
  this->crc = Frame::crc32(*this);
}

std::vector<uint8_t> Frame::serialize() const {
  // Create data array
  const std::size_t output_len = 2 * MAC_LEN
    + sizeof(this->type)
    + this->payload.size()
    + sizeof(this->crc);
  std::vector<uint8_t> output(output_len);
  auto iter = output.begin();

  // Copy MAC addresses
  std::copy(this->dst.begin(), this->dst.end(), iter);
  iter += dst.size();
  std::copy(this->src.begin(), this->src.end(), iter);
  iter += dst.size();

  // Copy EtherType
  *(iter++) = static_cast<uint8_t>(static_cast<uint16_t>(this->type) >> 8);
  *(iter++) = static_cast<uint8_t>(static_cast<uint16_t>(this->type) & 0xff);

  // Copy payload
  std::copy(this->payload.begin(), this->payload.end(), iter);
  iter += payload.size();

  // Copy CRC32
  for (std::size_t i = 0; i < sizeof(this->crc); ++i) {
    *(iter++) = static_cast<uint8_t>((this->crc >> (8 * i)) & 0xff);
  }

  return output;
}

bool Frame::isValid() const {
  return this->crc == Frame::crc32(*this);
}

void Frame::setDst(const MacAddr &new_dst) {
  this->dst = new_dst;
  this->updateCRC();
}

void Frame::setSrc(const MacAddr &new_src) {
  this->src = new_src;
  this->updateCRC();
}

void Frame::setType(const EtherType &new_type) {
  this->type = new_type;
  this->updateCRC();
}

void Frame::setPayload(const std::vector<uint8_t> &new_payload, const bool recalc_crc) {
  if(PAYLOAD_LEN_MAX < new_payload.size()){
    throw std::runtime_error("Payload too large");
  }
  if(new_payload.size() < PAYLOAD_LEN_MIN){
    throw std::runtime_error("Payload too small");
  }
  this->payload = new_payload;
  if(recalc_crc){
    this->updateCRC();
  }
}

MacAddr Frame::getDst() const {
  return this->dst;
}

MacAddr Frame::getSrc() const {
  return this->src;
}

Frame::EtherType Frame::getType() const {
  return this->type;
}

const std::vector<uint8_t>& Frame::getPayload() const {
  return this->payload;
}

uint32_t Frame::getCRC() const {
  return this->crc;
}

void Frame::updateCRC() {
  this->crc = Frame::crc32(*this);
}

constexpr std::array<uint32_t, 256> Frame::generateCRC32Table() {
  // CRC32 table generation for improved computation speed.
  // See the following for more info
  // https://web.archive.org/web/20130715065157/http://gnuradio.org/redmine/projects/gnuradio/repository/revisions/1cb52da49230c64c3719b4ab944ba1cf5a9abb92/entry/gr-digital/lib/digital_crc32.cc
  // https://gist.github.com/timepp/1f678e200d9e0f2a043a9ec6b3690635

  std::array<uint32_t, 256> table{};
  for (uint32_t i = 0; i < table.size(); ++i) {
    uint32_t crc = i;
    for (std::size_t j = 0; j < 8; ++j) {
      if (crc & 1) {
        crc = CRC32_POLYNOMIAL ^ (crc >> 1);
      } else {
        crc >>= 1;
      }
    }
    table[i] = crc;
  }
  return table;
}

uint32_t Frame::crc32(const Frame &frame) {
  // Generates the table once and reuses it for all CRC32 calculations
  static constexpr std::array<uint32_t, 256> CRC32_TABLE = generateCRC32Table();

  // Arrange data into spans for lightweight, easier, and safer access
  // Most of this will likely be optimized out
  std::array<std::span<const uint8_t>, 4> data;
  data[0] = std::span<const uint8_t>(frame.dst);
  data[1] = std::span<const uint8_t>(frame.src);
  const uint16_t ethertype = static_cast<uint16_t>(frame.type);
  const std::array<uint8_t, 2> ethertype_bytes = {
      static_cast<uint8_t>((ethertype >> 8) & 0xff),
      static_cast<uint8_t>((ethertype) & 0xff)};
  data[2] = std::span<const uint8_t>(ethertype_bytes);
  data[3] = std::span<const uint8_t>(frame.payload);

  // Calculates CRC32 from multiple spans, useful to remove copying, etc
  uint32_t crc = CRC32_INITIAL;
  std::span<const std::span<const uint8_t>> spans(data);
  for (const std::span<const uint8_t> &span : spans) {
    for (const uint8_t byte : span) {
      uint8_t index = (crc ^ byte) & 0xFF;
      crc = (crc >> 8) ^ CRC32_TABLE[index];
    }
  }
  return crc ^ CRC32_FINAL_XOR;
}

} // namespace Ethernet
