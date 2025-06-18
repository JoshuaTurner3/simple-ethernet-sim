#include "Protocol.hpp"
#include <cstring>
#include <limits>
#include <stdexcept>

namespace Protocol {

/* Helper Functions */
std::vector<uint8_t> packMsg(const MsgType t, const uint8_t id,
                             const std::vector<uint8_t> &data) {
  // Size check
  if (std::numeric_limits<uint16_t>::max() < data.size()) {
    std::runtime_error("Attempt to pack message with more data than allowed");
  }

  // Create buffer
  std::vector<uint8_t> bytes(MSG_LEN_MIN + data.size());
  auto iter = bytes.begin();

  // Pack message type
  *(iter++) = static_cast<uint8_t>(t);

  // Pack message id
  *(iter++) = id;

  // Pack message length
  *(iter++) = (static_cast<uint16_t>(data.size()) >> 8) & 0xff;
  *(iter++) = static_cast<uint16_t>(data.size()) & 0xff;

  // Pack message data
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
  std::memmove(&msg.header.type, &(*iter), sizeof(msg.header.type));
  iter += sizeof(msg.header.type);

  // Copy message id
  std::memmove(&msg.header.id, &(*iter), sizeof(msg.header.id));
  iter += sizeof(msg.header.id);

  // Copy message length
  std::memmove(&msg.header.len, &(*iter), sizeof(msg.header.len));
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

/* Host */
Host::Host(Driver &driver) : driver(driver) {}

bool Host::poll() {
  // Get message
  std::vector<uint8_t> bytes;
  if (!this->driver.recv(bytes)) {
    return false;
  }
  const Msg msg = unpackMsg(bytes);

  // Handler dispatch
  switch (msg.header.type) {
  case MsgType::COMMAND:
    throw std::runtime_error("Host received command");
  case MsgType::RESPONSE:
    this->handleResponse(msg);
    break;
  case MsgType::STREAM:
    this->handleStream(msg);
    break;
  case MsgType::ERROR:
    this->handleError(msg);
    break;
  default:
    throw std::runtime_error("Unrecognized message type");
  }

  return true;
}

void Host::sendCommand(const uint8_t cmd_id, const std::vector<uint8_t> &data) {
  const std::vector<uint8_t> bytes = packMsg(MsgType::COMMAND, cmd_id, data);
  this->driver.send(bytes);
}

void Host::handleResponse(const Msg& msg){
  // TODO
}

void Host::handleStream(const Msg& msg){
  // TODO
}

void Host::handleError(const Msg& msg){
  // TODO
}

/* Device */
Device::Device(Driver &driver)
    : driver(driver) {}

} // namespace Protocol
