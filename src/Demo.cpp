#include "ProtocolDevice.hpp"
#include "ProtocolHost.hpp"
#include <atomic>
#include <chrono>
#include <cstdio>
#include <thread>

using namespace Ethernet;
using namespace Protocol;

int main() {
  // MAC addresses
  const MacAddr MAC_A = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
  const MacAddr MAC_B = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

  // Simulate physical layer
  Driver host_eth(MAC_A);
  Driver dev_eth(MAC_B);
  Driver::link(host_eth, dev_eth);

  // Devices
  Host host(host_eth);
  Device dev(dev_eth);

  // Atomic bool to coordinate program halt
  std::atomic<bool> run{true};

  /* DEVICE */
  // New thread for Device
  std::thread devThread([&] {
    while (run.load()) {
      dev.poll();
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  });

  /* HOST */
  // The main thread is the host
  auto hostStep = [&] {
    host.poll(); // check for responses or streams
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  };

  // PING
  host.sendCommand(CmdID::PING, {});
  for (int i = 0; i < 20; ++i)
    hostStep();

  // START_STREAM
  host.sendCommand(CmdID::START_STREAM, {});
  for (int i = 0; i < 100; ++i)
    hostStep(); // one second @10 ms steps

  // STOP_STREAM
  host.sendCommand(CmdID::STOP_STREAM, {});
  for (int i = 0; i < 20; ++i)
    hostStep();

  // UNKNOWN_COMMAND
  host.sendCommand(static_cast<CmdID>(0x99), {});
  for (int i = 0; i < 20; ++i)
    hostStep();

  /* Shutdown -------------------------------------------------------- */
  run.store(false);
  devThread.join();

  std::puts("\nDemo complete (single-producer single-consumer)");
  return 0;
}
