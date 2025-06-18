
#ifndef PROTOCOL_HOST_HPP
#define PROTOCOL_HOST_HPP

#include "Protocol.hpp"

namespace Protocol {

class Host {
public:
  Host() = delete;
  ~Host() = default;
  Host(const Host&) = delete;
  Host& operator=(const Host&) = delete;
  explicit Host(Driver &driver);

  bool poll();
  void sendCommand(const uint8_t cmd_id, const std::vector<uint8_t>& data);

private:
  /* Handlers */
  void handleResponse(const Msg& msg);
  void handleStream(const Msg& msg);
  void handleError(const Msg& msg);

  /* Data */
  Driver& driver;
};

} // namespace Protocol

#endif // PROTOCOL_HOST_HPP
