#include "EthernetDriver.hpp"
#include "EthernetFrame.hpp"
#include <stdexcept>

namespace Ethernet {

Driver::Driver(const MacAddr &mac_self) : mac_self(mac_self) {}

void Driver::setMacAddr(const MacAddr& new_mac_self){
  this->mac_self = new_mac_self;
}

MacAddr Driver::getMacAddr() const {
  return this->mac_self;
}

void Driver::setPeerMacAddr(const MacAddr& new_mac_peer){
  this->mac_peer = new_mac_peer;
}

MacAddr Driver::getPeerMacAddr() const{
  return this->mac_peer;
}

void Driver::send(const std::vector<uint8_t> &data,
                  const Frame::EtherType type) {
  if (!this->txQueue) {
    throw std::logic_error("Driver not linked");
  }
  Frame frame(this->mac_peer, this->mac_self, type, data);
  if(this->error_injection){
    this->corrupt(frame);
  }
  std::lock_guard<std::mutex> lock(*this->txMutex);
  this->txQueue->push_back(std::move(frame.serialize()));
}

bool Driver::recv(std::vector<uint8_t>& output){
  std::lock_guard<std::mutex> lock(this->rxMutex);
  if(this->rxQueue.empty()){
    return false;
  }

  // Receive frame
  Frame frame = Frame(this->rxQueue.front());
  this->rxQueue.pop_front();

  // Ensure the destination MAC is correct
  if(frame.getDst() != this->mac_self){
    throw std::runtime_error("Driver received frame for incorrect destination MAC");
  }

  // Return
  output = std::move(frame.getPayload());
  return true;
}

bool Driver::hasPending() const {
  return !this->rxQueue.empty();
}

void Driver::setErrorInjection(const bool enable){
  this->error_injection = enable;
}

void Driver::link(Driver &a, Driver &b){
  // MAC exchange
  a.mac_peer = b.mac_self;
  b.mac_peer = a.mac_self;
  // Mutex exchange
  a.txMutex = &b.rxMutex;
  b.txMutex = &a.rxMutex;
  // Queue exchange
  a.txQueue = &b.rxQueue;
  b.txQueue = &a.rxQueue;
}

void Driver::corrupt(Frame& frame){
  auto payload = frame.getPayload();
  const std::size_t byte_index = rand() % payload.size();
  const uint8_t bit_index = 0x01 << (rand() % 7);
  payload[byte_index] ^= bit_index;
  frame.setPayload(payload, false);
}

} // namespace Ethernet
