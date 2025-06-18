
#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include "EthernetDriver.hpp"

namespace Protocol {

/* Types */
using Driver = Ethernet::Driver;
using MacAddr = Ethernet::MacAddr;
enum class MsgType : uint8_t {
  COMMAND = 0x01,
  RESPONSE = 0x02,
  STREAM = 0x03,
  ERROR = 0x04,
};
struct Msg {
  struct Header {
    MsgType type;
    uint8_t id;
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
static std::vector<uint8_t> packMsg(const MsgType t, const uint8_t id,
                                    const std::vector<uint8_t> &data);
static Msg unpackMsg(const std::vector<uint8_t> &data);

} // namespace Protocol

#endif // PROTOCOL_HPP
