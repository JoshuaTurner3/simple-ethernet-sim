#include "EthernetDriver.hpp"
#include <catch2/catch_all.hpp>

/*
 * Ethernet Driver tests
 */
using namespace Ethernet;

static const MacAddr MAC_A{0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
static const MacAddr MAC_B{0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

/* ------------------------------------------------------------ */
TEST_CASE("Driver round‑trip min payload") {
  Driver host(MAC_A);
  Driver dev(MAC_B);
  Driver::link(host, dev);

  std::vector<uint8_t> tx(Frame::PAYLOAD_LEN_MIN, 0x42);
  host.send(tx);

  std::vector<uint8_t> rx;
  REQUIRE(dev.recv(rx));
  REQUIRE(rx == tx);
}

/* ------------------------------------------------------------ */
TEST_CASE("Non‑blocking recv returns false when empty") {
  Driver node(MAC_A);
  std::vector<uint8_t> buf;
  REQUIRE_FALSE(node.recv(buf));
}

/* ------------------------------------------------------------ */
TEST_CASE("Payload size bounds enforced") {
  Driver a(MAC_A);
  Driver b(MAC_B);
  Driver::link(a, b);

  std::vector<uint8_t> small(Frame::PAYLOAD_LEN_MIN - 1, 0x00);
  REQUIRE_THROWS_AS(a.send(small), std::runtime_error);

  std::vector<uint8_t> large(Frame::PAYLOAD_LEN_MAX + 1, 0x00);
  REQUIRE_THROWS_AS(a.send(large), std::runtime_error);
}

/* ------------------------------------------------------------ */
TEST_CASE("Error injection flips bit") {
  Driver a(MAC_A);
  Driver b(MAC_B);
  Driver::link(a, b);
  a.setErrorInjection(true);

  std::vector<uint8_t> tx(Frame::PAYLOAD_LEN_MIN, 0xAA);
  a.send(tx);

  std::vector<uint8_t> rx;
  REQUIRE_THROWS_AS(b.recv(rx), std::runtime_error);
}
