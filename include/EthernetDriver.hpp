#ifndef ETHERNET_DRIVER_HPP
#define ETHERNET_DRIVER_HPP

#include "EthernetFrame.hpp"
#include <deque>
#include <memory>
#include <vector>

namespace Ethernet {

class Driver {
public:
  /* Types */
  using MacAddr = Frame::MacAddr;
  using ByteQueue = std::deque<Frame>;
  using ByteQueuePointer = std::shared_ptr<ByteQueue>;

  Driver() = default;
  Driver(const MacAddr& rx, const MacAddr& tx);
  ~Driver() = default;
  Driver(const Driver &) = default;
  Driver &operator=(const Driver &) = default;
  friend bool operator==(const Driver &, const Driver &) = default;

  void setRxMacAddr(const MacAddr& new_rx);
  void setTxMacAddr(const MacAddr& new_tx);

  void sendFrame(const Frame& frame);
  bool recvFrame(const Frame& frame);

  bool hasPending() const;

  void setErrorInjection(bool enable);

private:
  /* Function to flip a random byte in any sent packets, purely for testing purposes
   * @
   */
  static void corrupt(Frame& frame);

  /* Data */
  ByteQueue rxQueue{};
  ByteQueuePointer txQueue{nullptr};
  bool error_injection{false}
};

} // namespace Ethernet

#endif // ETHERNET_DRIVER_HPP
