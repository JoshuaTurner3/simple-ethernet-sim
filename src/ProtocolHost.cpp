#include "ProtocolHost.hpp"
#include "Protocol.hpp"
#include <stdexcept>

namespace Protocol {

/* Host */
Host::Host(Driver &driver) : driver(driver) {}

bool Host::poll() {
  // Get message
  std::vector<uint8_t> bytes;
  if (!this->driver.recv(bytes)) {
    return false;
  }
  const Msg msg = Protocol::unpackMsg(bytes);

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

void Host::handleResponse(const Msg &msg) {
  // TODO
}

void Host::handleStream(const Msg &msg) {
  // TODO
}

void Host::handleError(const Msg &msg) {
  // TODO
}

} // namespace Protocol
