#include "Protocol.hpp"
#include "EthernetFrame.hpp"
#include <cstring>
#include <limits>
#include <stdexcept>

namespace Protocol {

/* Helper Functions */
std::vector<uint8_t> packMsg(const MsgType t, const ID id,
                             const std::vector<uint8_t> &data) {
  // Size check
  if (std::numeric_limits<uint16_t>::max() < data.size()) {
    throw std::runtime_error(
        "Attempt to pack message with more data than allowed");
  }

  // Buffer size
  const std::size_t size_total = MSG_LEN_MIN + data.size();
  const std::size_t size_padding =
      size_total < Ethernet::Frame::PAYLOAD_LEN_MIN
          ? Ethernet::Frame::PAYLOAD_LEN_MIN - size_total
          : 0;

  // Create buffer
  std::vector<uint8_t> bytes(size_total + size_padding);
  auto iter = bytes.begin();

  // Pack message type
  *(iter++) = static_cast<uint8_t>(t);

  // Pack message id
  switch (t) {
  case MsgType::RESPONSE:
  case MsgType::COMMAND:
    *(iter++) = static_cast<uint8_t>(id.cmd_id);
    break;
  case MsgType::STREAM:
    *(iter++) = static_cast<uint8_t>(id.stream_id);
    break;
  case MsgType::ERROR:
    *(iter++) = static_cast<uint8_t>(id.error_id);
    break;
  default:
    throw std::runtime_error("Unknown message type");
    break;
  }

  // Pack message length
  *(iter++) = static_cast<uint16_t>(data.size()) & 0xff;
  *(iter++) = (static_cast<uint16_t>(data.size()) >> 8) & 0xff;

  // Pack message data
  std::fill_n(iter, size_padding, 0x00);
  iter += size_padding;
  std::copy(data.begin(), data.end(), iter);

  return bytes;
}

Msg unpackMsg(const std::vector<uint8_t> &bytes) {
  Msg msg;
  auto iter = bytes.begin();
  if (bytes.size() < MSG_LEN_MIN) {
    throw std::runtime_error("Packed message data below minimum");
  }

  // Copy message type
  msg.header.type = static_cast<MsgType>(*(iter++));

  // Copy message id
  switch (msg.header.type) {
  case MsgType::RESPONSE:
  case MsgType::COMMAND:
    msg.header.id.cmd_id = static_cast<CmdID>(*(iter++));
    break;
  case MsgType::STREAM:
    msg.header.id.stream_id = static_cast<StreamID>(*(iter++));
    break;
  case MsgType::ERROR:
    msg.header.id.error_id = static_cast<ErrorID>(*(iter++));
    break;
  default:
    throw std::runtime_error("Unknown message type");
    break;
  }

  // Copy message length
  msg.header.len = (static_cast<uint16_t>(*iter) << 8) |
                   (static_cast<uint16_t>(*(iter + 1)));
  iter += sizeof(msg.header.len);

  // Check message length
  if (bytes.size() - MSG_LEN_MIN < msg.header.len) {
    throw std::runtime_error("Message length longer than remaining data bytes");
  }

  // We add padding to the front of the message to meet the minimum requirements
  // of an ethernet transfer; therefore we will only take the specified data
  // amount from the header off the end of the data stream/buffer
  iter = bytes.end() - msg.header.len;
  msg.data.resize(msg.header.len);
  std::copy(iter, bytes.end(), msg.data.begin());

  return msg;
}

} // namespace Protocol
