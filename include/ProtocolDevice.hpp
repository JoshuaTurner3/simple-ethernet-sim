
#ifndef PROTOCOL_DEVICE_HPP
#define PROTOCOL_DEVICE_HPP

#include "Protocol.hpp"

namespace Protocol {

class Device {
public:
  Device() = delete;
  ~Device() = default;
  Device(const Device &) = delete;
  Device &operator=(const Device &) = delete;
  explicit Device(Driver &driver);

  bool poll();

  void sendResponse(const CmdID cmd_id, const std::vector<uint8_t> &data);
  void sendStream(const StreamID stream_id, const std::vector<uint8_t> &data);
  void sendError(const ErrorID code);

private:
  /* Handlers */
  void handleCommand(const Msg &msg);

  /* Data */
  Driver &driver;
  bool send_stream{false};
};

} // namespace Protocol

#endif // PROTOCOL_DEVICE_HPP
