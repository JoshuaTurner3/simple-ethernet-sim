#include "ProtocolDevice.hpp"
#include "Protocol.hpp"
#include <stdexcept>

namespace Protocol {
/* Device */
Device::Device(Driver &driver)
    : driver(driver) {}

bool Device::poll(){
  // Get message
  std::vector<uint8_t> bytes;
  if(!this->driver.recv(bytes)){
    return false;
  }
  const Msg msg = Protocol::unpackMsg(bytes);

  // Handler dispatch
  switch (msg.header.type) {
  case MsgType::COMMAND:
    this->handleCommand(msg);
    break;
  case MsgType::RESPONSE:
    throw std::runtime_error("Device received response");
    break;
  case MsgType::STREAM:
    throw std::runtime_error("Device received stream");
    break;
  case MsgType::ERROR:
    throw std::runtime_error("Device received error");
    break;
  default:
    throw std::runtime_error("Unrecognized message type");
  }

  return true;
}

void Device::sendResponse(const uint8_t cmd_id, const std::vector<uint8_t> &data){
  const std::vector<uint8_t> bytes = packMsg(MsgType::RESPONSE, cmd_id, data);
  this->driver.send(bytes);
}

void Device::sendStream(const uint8_t stream_id, const std::vector<uint8_t> &data){
  const std::vector<uint8_t> bytes = packMsg(MsgType::STREAM, stream_id, data);
  this->driver.send(bytes);
}

void Device::sendError(const uint8_t code){
  const std::vector<uint8_t> bytes = packMsg(MsgType::ERROR, code, {});
  this->driver.send(bytes);
}

void Device::handleCommand(const Msg& msg){
  // TODO
}

} // namespace Protocol
