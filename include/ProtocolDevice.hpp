
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

  void sendResponse(const uint8_t cmd_id, const std::vector<uint8_t> &data);
  void sendStream(const uint8_t stream_id, const std::vector<uint8_t> &data);
  void sendError(const uint8_t code);

private:
  /* Handlers */
  void handleCommand(const Msg &msg);

  /* Data */
  Driver &driver;
};

} // namespace Protocol

#endif // PROTOCOL_DEVICE_HPP
