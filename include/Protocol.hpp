
#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include "EthernetDriver.hpp"

namespace Protocol {

/* Types */
using Driver = Ethernet::Driver;
using MacAddr = Ethernet::MacAddr;

enum class CmdID : uint8_t {
  PING = 0x01,
  START_STREAM = 0x02,
  STOP_STREAM = 0x03,
};

enum class StreamID : uint8_t {
  TELEMETRY = 0x01,
};

enum class ErrorID : uint8_t {
  UNKNOWN_CMD = 0x01,
  BAD_PAYLOAD = 0x02,
};
union ID {
  CmdID cmd_id;
  StreamID stream_id;
  ErrorID error_id;
};

enum class MsgType : uint8_t {
  COMMAND = 0x01,
  RESPONSE = 0x02,
  STREAM = 0x03,
  ERROR = 0x04,
};

struct Msg {
  struct Header {
    MsgType type;
    ID id;
    uint16_t len;
  };
  Header header;
  std::vector<uint8_t> data;
};

/* Constants */
// The bare minimum we need to verify size. I'm not using sizeof(Msg::Header)
// since there is likely padding and packed messages will have that removed
static constexpr std::size_t MSG_LEN_MIN = sizeof(Msg::Header::type) +
                                           sizeof(Msg::Header::id) +
                                           sizeof(Msg::Header::len);

/* Helper functions */
std::vector<uint8_t> packMsg(const MsgType t, const ID id,
                             const std::vector<uint8_t> &data);

Msg unpackMsg(const std::vector<uint8_t> &data);

} // namespace Protocol

#endif // PROTOCOL_HPP
