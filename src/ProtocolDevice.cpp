#include "ProtocolDevice.hpp"
#include "Protocol.hpp"
#include <stdexcept>

namespace Protocol {
/* Device */
Device::Device(Driver &driver) : driver(driver) {}

bool Device::poll() {
  // Check for any received commands
  std::vector<uint8_t> bytes{};
  if (this->driver.recv(bytes)) {
    const Msg msg = Protocol::unpackMsg(bytes);
    // Handler dispatch
    switch (msg.header.type) {
    case MsgType::COMMAND:
      this->handleCommand(msg);
      break;
    case MsgType::RESPONSE:
      throw std::runtime_error("Device received response");
    case MsgType::STREAM:
      throw std::runtime_error("Device received stream");
    case MsgType::ERROR:
      throw std::runtime_error("Device received error");
    default:
      throw std::runtime_error("Unrecognized message type");
    }
  }

  // Send stream data if enabled
  static uint32_t num_frames_sent = 0;
  if(this->send_stream){
    std::vector<uint8_t> payload;
    payload.resize(sizeof(num_frames_sent));
    for(std::size_t i = 0; i < sizeof(num_frames_sent); ++i){
      payload[i] = static_cast<uint8_t>((num_frames_sent >> (i*8)) & 0xff);
    }
    this->sendStream(StreamID::TELEMETRY, payload);
    ++num_frames_sent;
  }

  return true;
}

void Device::sendResponse(const CmdID cmd_id,
                          const std::vector<uint8_t> &data) {
  ID id;
  id.cmd_id = cmd_id;
  const std::vector<uint8_t> bytes = packMsg(MsgType::RESPONSE, id, data);
  this->driver.send(bytes);
}

void Device::sendStream(const StreamID stream_id,
                        const std::vector<uint8_t> &data) {
  ID id;
  id.stream_id = stream_id;
  const std::vector<uint8_t> bytes = packMsg(MsgType::STREAM, id, data);
  this->driver.send(bytes);
}

void Device::sendError(const ErrorID error_id) {
  ID id;
  id.error_id = error_id;
  const std::vector<uint8_t> bytes = packMsg(MsgType::ERROR, id, {});
  this->driver.send(bytes);
}

void Device::handleCommand(const Msg &msg) {
  const CmdID id = msg.header.id.cmd_id;
  switch (msg.header.id.cmd_id) {
    case CmdID::PING: {
      const std::vector<uint8_t> pong = {'P','O','N','G'};
      sendResponse(id, pong);
      break;
    }
    case CmdID::START_STREAM: {
      this->send_stream = true;
      const std::vector<uint8_t> ok = {'O','K'};
      sendResponse(id, ok);
      break;
    }
    case CmdID::STOP_STREAM: {
      this->send_stream = false;
      const std::vector<uint8_t> ok = {'O','K'};
      sendResponse(id, ok);
      break;
    }
    default:
      this->sendError(ErrorID::UNKNOWN_CMD);
      break;
  }
}

} // namespace Protocol
