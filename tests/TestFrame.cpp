#include <catch2/catch_all.hpp>
#include <numeric>
#include "EthernetFrame.hpp"

/*
 * Ethernet Frame tests
 */
using namespace Ethernet;

static const MacAddr MAC_A{0x00,0x11,0x22,0x33,0x44,0x55};
static const MacAddr MAC_B{0xAA,0xBB,0xCC,0xDD,0xEE,0xFF};

/* ------------------------------------------------------------ */
TEST_CASE("Round‑trip minimum payload (46 B)"){
    std::vector<uint8_t> payload(Frame::PAYLOAD_LEN_MIN, 0x42);
    Frame f_tx(MAC_A, MAC_B, Frame::EtherType::IPV4, payload);
    auto bytes = f_tx.serialize();
    Frame f_rx(bytes);
    REQUIRE(f_rx == f_tx);
    REQUIRE(f_rx.isValid());
}

/* ------------------------------------------------------------ */
TEST_CASE("Round‑trip variable payload sizes"){
    // Test a spectrum: 46, 128, 512, 1500 bytes
    const std::vector<std::size_t> sizes = {46, 128, 512, 1500};
    for(auto sz : sizes){
        INFO("payload=" << sz);
        std::vector<uint8_t> pl(sz);
        std::iota(pl.begin(), pl.end(), 0);   // deterministic pattern
        Frame tx(MAC_B, MAC_A, Frame::EtherType::IPV6, pl);
        Frame rx(tx.serialize());
        REQUIRE(rx == tx);
        REQUIRE(rx.isValid());
    }
}

/* ------------------------------------------------------------ */
TEST_CASE("CRC detects corruption"){
    std::vector<uint8_t> payload(Frame::PAYLOAD_LEN_MIN, 0x00);
    Frame good(MAC_A, MAC_B, Frame::EtherType::ARP, payload);
    auto bytes = good.serialize();
    // Flip one bit in the payload
    bytes[2*MAC_LEN + sizeof(Frame::EtherType) + 3] ^= 0x01;
    REQUIRE_THROWS_AS(Frame(bytes), std::runtime_error);
}

/* ------------------------------------------------------------ */
TEST_CASE("Payload size enforcement"){
    // Too small
    std::vector<uint8_t> tiny(Frame::PAYLOAD_LEN_MIN - 1, 0x00);
    REQUIRE_THROWS_AS(Frame(MAC_A, MAC_B, Frame::EtherType::IPV4, tiny), std::runtime_error);
    // Too big
    std::vector<uint8_t> huge(Frame::PAYLOAD_LEN_MAX + 1, 0x00);
    REQUIRE_THROWS_AS(Frame(MAC_A, MAC_B, Frame::EtherType::IPV4, huge), std::runtime_error);
}

/* ------------------------------------------------------------ */
TEST_CASE("Mutators preserve validity"){
    std::vector<uint8_t> payload(Frame::PAYLOAD_LEN_MIN, 0xFF);
    Frame f(MAC_A, MAC_B, Frame::EtherType::IPV4, payload);
    f.setDst(MAC_B);
    f.setSrc(MAC_A);
    f.setType(Frame::EtherType::IPV6);
    std::vector<uint8_t> new_pl(128, 0xAA);
    f.setPayload(new_pl);
    REQUIRE(f.isValid());
}

/* ------------------------------------------------------------ */
TEST_CASE("Serialize length correctness"){
    std::vector<uint8_t> payload(100);
    Frame f(MAC_A, MAC_B, Frame::EtherType::IPV4, payload);
    auto buf = f.serialize();
    REQUIRE(buf.size() == 2*MAC_LEN + sizeof(Frame::EtherType) + payload.size() + sizeof(uint32_t));
}
