#ifndef ETHERNET_FRAME_HPP
#define ETHERNET_FRAME_HPP

#include <array>
#include <cstdint>
#include <span>
#include <vector>

namespace Ethernet {

/* General Constants/Types */
constexpr std::size_t MAC_LEN = 6;
using MacAddr = std::array<uint8_t, MAC_LEN>;

class Frame {
public:
  /* Constants */
  // The following constants select CRC32 constants
  // See CRC-32 @ https://en.wikipedia.org/wiki/Cyclic_redundancy_check
  // We are using reversed since the Ethernet Frame standard sends the CRC in big-endian
  // Normal:   0x04c11db7
  // Reversed: 0xedb88320
  // Reciprocal: 0xdb710641
  static constexpr uint32_t CRC32_POLYNOMIAL = 0xEDB88320;
  static constexpr uint32_t CRC32_INITIAL = 0xFFFFFFFF;
  static constexpr uint32_t CRC32_FINAL_XOR = 0xFFFFFFFF;
  static constexpr std::size_t FRAME_LEN_MIN = 64;
  static constexpr std::size_t FRAME_LEN_MAX = 1518;
  static constexpr std::size_t PAYLOAD_LEN_MIN = 46;
  static constexpr std::size_t PAYLOAD_LEN_MAX = 1500;

  /* Types */
  enum class EtherType : uint16_t {
    IPV4 = 0x0800,
    IPV6 = 0x86DD,
    ARP = 0x0806,
  };

  /* Default constructor
   */
  Frame();

  /* Default deconstructor
   */
  ~Frame() = default;

  /* Default copy constructor
   * @param Frame The frame to copy from
   */
  Frame(const Frame &) = default;

  /* Default assignment operator
   * @param Frame The frame to copy from
   * @return Reference to the newly created copy frame
   */
  Frame &operator=(const Frame &) = default;

  /* Alternate constructor that parses a byte buffer appropriately to construct
   * the frame
   * @param data The byte buffer (span) to parse
   */
  Frame(std::span<const uint8_t> data);

  /* Alternate constructor that allows manual creation of Ethernet Frame
   * properties and fields
   * @param dst The destination MAC Address
   * @param src The source MAC Address
   * @param type The EtherType of the frame
   * @param payload The frame's data
   */
  Frame(const MacAddr &dst, const MacAddr &src, const EtherType type,
        const std::vector<uint8_t> &payload);

  /* Equality operator between Frame types
   * @param lhs Left-hand frame
   * @param rhs Right-hand frame
   * @return True if frames have the same member data, false otherwise
   */
  friend bool operator==(const Frame &, const Frame &) = default;

  /* Serializes the ethernet frame into a byte buffer
   * @return Buffer of the ethernet frame's contents
   */
  [[nodiscard]] std::vector<uint8_t> serialize() const;

  /* Checks whether the CRC32 of the frame matches the expected
   * @return True if the CRC32 is valid, false otherwise
   */
  [[nodiscard]] bool isValid() const;

  /* Updates the destination MAC address
   * @param new_dst The new destination MAC address for the frame
   * @return none
   */
  void setDst(const MacAddr &new_dst);

  /* Updates the source MAC address
   * @param new_src The new source MAC address for the frame
   * @return none
   */
  void setSrc(const MacAddr &new_src);

  /* Updates the EtherType
   * @param new_type The new ethertype for the frame
   * @return none
   */
  void setType(const EtherType &new_type);

  /* Updates the frame's payload
   * @param new_src The new payload for the frame
   * @param recalc_crc Whether or not to go ahead and recalculate the CRC32 for
   * the frame. In reality you would always want this to be true; however, for
   * testing purposes a manual disable has been added fault injection.
   * @return none
   */
  void setPayload(const std::vector<uint8_t> &new_payload, const bool recalc_crc = true);

  /* Gets the frame's destination MAC address
   * @return The destination MAC address
   */
  [[nodiscard]] MacAddr getDst() const;

  /* Gets the frame's source MAC address
   * @return The source MAC address
   */
  [[nodiscard]] MacAddr getSrc() const;

  /* Gets the frame's EtherType code
   * @return The frame's EtherType code
   */
  [[nodiscard]] EtherType getType() const;

  /* Gets the frame's payload
   * @return The frame's payload
   */
  [[nodiscard]] const std::vector<uint8_t>& getPayload() const;

  /* Gets the frame's CRC32
   * @return The frame's CRC32
   */
  [[nodiscard]] uint32_t getCRC() const;

private:
  /* Private member function for updating the crc32 of the ethernet frame,
   * should be called by internal setters.
   */
  void updateCRC();

  static constexpr std::array<uint32_t, 256> generateCRC32Table();

  /* Calculates the CRC32 for the Ethernet Frame span
   */
  static uint32_t crc32(const Frame &frame);

  /* Data */
  MacAddr dst{};
  MacAddr src{};
  EtherType type{EtherType::IPV4};
  std::vector<uint8_t> payload{};
  uint32_t crc{0};
};

} // namespace Ethernet

#endif // ETHERNET_FRAME_HPP
