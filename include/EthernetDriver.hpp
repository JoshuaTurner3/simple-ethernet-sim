#ifndef ETHERNET_DRIVER_HPP
#define ETHERNET_DRIVER_HPP

#include "EthernetFrame.hpp"
#include <deque>

namespace Ethernet {

class Driver {
public:
  /* Types */
  using ByteQueue = std::deque<std::vector<uint8_t>>;

  /* Default constructor, deleted to require MAC address on creation
   */
  Driver() = delete;

  /* Alternate constructor
   * @param mac The MAC address for the host driver
   */
  explicit Driver(const MacAddr &mac_self);

  /* Default deconstructor
   */
  ~Driver() = default;

  /* Copy constructor, deleted to prevent queue reference/pointer duplication
   */
  Driver(const Driver &) = delete;

  /* Assignment operator, delete to prevent queue reference/pointer duplication
   */
  Driver &operator=(const Driver &) = delete;

  /* Equality operator between Driver types
   * @param lhs Left-hand Driver
   * @param rhs right-hand Driver
   * @return True if frames have the same member data, false otherwise
   */
  friend bool operator==(const Driver &, const Driver &) = default;

  /* Sets the MAC address for the driver
   * @param new_mac The new MAC address of the driver
   * @return none
   */
  void setMacAddr(const MacAddr &new_mac_self);

  /* Gets the MAC address of the driver
   * @return The MAC address of the driver
   */
  [[nodiscard]] MacAddr getMacAddr() const;

  /* Sets the MAC address for the driver to send to
   * @param new_mac The new MAC address for the driver to send to
   * @return none
   */
  void setPeerMacAddr(const MacAddr &new_mac_peer);

  /* Gets the MAC address of the driver's peer
   * @return The MAC address of the driver's peer
   */
  [[nodiscard]] MacAddr getPeerMacAddr() const;

  /* Sends a byte-array of data to the linked peer
   * @param data The byte-array of data to send
   * @param type The type of the frame, defaults to IPv4
   * @return none
   */
  void send(const std::vector<uint8_t> &data,
            const Frame::EtherType type = Frame::EtherType::IPV4);

  /* Receives a byte-array of data from the linked peer. Blocks by default.
   * @param output The received peer data. Unchanged if non-blocking and nothing
   * received.
   * @param block True by default, whether the call waits for data to be
   * received or not.
   * @return True if data was received, False otherwise
   */
  bool recv(std::vector<uint8_t> &output, const bool block = true);

  /* Checks whether the Driver's peer has sent data
   * @return True if data has been received, False othewise
   */
  bool hasPending() const;

  /* Whether or not to flip a random bit in all sent frames. Purely for testing
   * purposes.
   * @param enable True will flip bits in send frames, False will not.
   * @return none
   */
  void setErrorInjection(const bool enable);

  /* This function links 2 drivers together. It is purely for simulating the
   * tx/rx queues that would normally be handled by physical hardware. That is,
   * it is only for testing
   * @param a The 1st driver to link
   * @param b The 2nd driver to link
   */
  static void link(Driver &a, Driver &b);

private:
  /* Function to flip a random byte in any sent packets, purely for testing
   * purposes
   * @param frame The frame to flip a bit in
   */
  static void corrupt(Frame &frame);

  /* Data */
  MacAddr mac_self{};
  MacAddr mac_peer{};
  ByteQueue rxQueue{};
  ByteQueue* txQueue{nullptr};
  bool error_injection{false};
};

} // namespace Ethernet

#endif // ETHERNET_DRIVER_HPP
