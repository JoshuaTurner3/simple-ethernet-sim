#ifndef ETHERNET_HPP
#define ETHERNET_HPP

#include <array>
#include <cstdint>

namespace Ethernet {

/* General Constants/Types */
constexpr std::size_t MAC_LEN = 6;
using MacAddr = std::array<uint8_t, MAC_LEN>;

} // namespace Ethernet

#endif // ETHERNET_FRAME_HPP
