#include "ProtocolHost.hpp"
#include "Protocol.hpp"
#include <cstring>
#include <stdexcept>

namespace Protocol {

/* Host */
Host::Host(Driver &driver) : driver(driver) {}

bool Host::poll() {
  // Get message
  std::vector<uint8_t> bytes{};
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

void Host::sendCommand(const CmdID cmd_id, const std::vector<uint8_t> &data) {
  ID id;
  id.cmd_id = cmd_id;
  const std::vector<uint8_t> bytes = packMsg(MsgType::COMMAND, id, data);
  this->driver.send(bytes);
}

void Host::handleResponse(const Msg &msg) {
  std::printf("[HOST] Response id=%u len=%u: ",
              static_cast<uint8_t>(msg.header.id.cmd_id), msg.header.len);
  for (const uint8_t byte : msg.data) {
    std::printf("%c", std::isprint(byte) ? byte : '.');
  }
  std::puts("");
}

void Host::handleStream(const Msg &msg) {
  const StreamID id = msg.header.id.stream_id;
  switch (id) {
  case StreamID::TELEMETRY:
    uint32_t count;
    std::memcpy(&count, msg.data.data(), sizeof(count));
    std::printf("[HOST] Stream count: %u\n", count);
    break;
  default:
    throw std::runtime_error("Unknown StreamID code");
  }
}

void Host::handleError(const Msg &msg) {
  std::printf("[HOST] ERROR code=%u\n", static_cast<uint8_t>(msg.header.id.error_id));
}

} // namespace Protocol
