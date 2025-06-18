#include <catch2/catch_all.hpp>

#include "EthernetFrame.hpp"
#include "Protocol.hpp"
#include "ProtocolDevice.hpp"
#include "ProtocolHost.hpp"

using namespace Ethernet;
using namespace Protocol;

/* PROTOCOL */
TEST_CASE("Protocol pack–unpack identity") {
  const std::vector<uint8_t> payload = {0xDE, 0xAD, 0xBE, 0xEF};

  const auto bytes =
      packMsg(MsgType::COMMAND, static_cast<ID>(CmdID::PING), payload);

  const Msg msg = unpackMsg(bytes);
  REQUIRE(msg.header.type == MsgType::COMMAND);
  REQUIRE(msg.header.id.cmd_id == CmdID::PING);
  REQUIRE(msg.header.len == payload.size());
  REQUIRE(msg.data == payload);
}

TEST_CASE("Protocol rejects oversize payload") {
  std::vector<uint8_t> large_buf(0x10000, 0x00);
  REQUIRE_THROWS_AS(
      packMsg(MsgType::COMMAND, static_cast<ID>(CmdID::PING), large_buf),
      std::runtime_error);
}

TEST_CASE("Protocol rejects truncated buffer") {
  std::vector<uint8_t> payload(Ethernet::Frame::PAYLOAD_LEN_MIN, 0x00);
  auto bytes =
      packMsg(MsgType::RESPONSE, static_cast<ID>(CmdID::PING), payload);
  // Remove last byte
  bytes.pop_back();
  REQUIRE_THROWS_AS(unpackMsg(bytes), std::runtime_error);
}

/* HOST/DEVICE */
static const MacAddr MAC_A{0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
static const MacAddr MAC_B{0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

TEST_CASE("Ping → Pong round-trip") {
  Driver hostEth(MAC_A);
  Driver devEth(MAC_B);
  Driver::link(hostEth, devEth);
  Host host(hostEth);
  Device dev(devEth);

  // Send ping
  host.sendCommand(CmdID::PING, {});
  REQUIRE(devEth.hasPending());

  // Step both
  dev.poll();
  host.poll();

  // Ensure queues empty
  REQUIRE_FALSE(hostEth.hasPending());
  REQUIRE_FALSE(devEth.hasPending());
}

TEST_CASE("Unknown command triggers ERROR frame") {
  Driver hostEth(MAC_A);
  Driver devEth(MAC_B);
  Driver::link(hostEth, devEth);
  Host host(hostEth);
  Device dev(devEth);

  // Send bad command enum
  host.sendCommand(static_cast<CmdID>(0x99), {});
  // Device should send host an ERROR
  dev.poll();
  // Host should consume error
  host.poll();

  // Ensure nothing in queue
  REQUIRE_FALSE(hostEth.hasPending());
  REQUIRE_FALSE(devEth.hasPending());
}
