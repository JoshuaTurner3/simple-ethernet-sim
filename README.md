# Simple Ethernet Driver/Protocol Simulation

A small C++23 project that models Ethernet frames, a driver queue, and a minimal application protocol. Everything runs in user space; no special hardware or kernel access is required.

## Quickstart

Requirements:
- Linux (Only tested on Linux; however, Windows should also work)
- A C++23 Compiler (Tested with: GCC 15, Clang 20)
- CMake >= 3.15
- Catch2

The program may be cloned and compiled using the following commands:

```bash
# To clone the repository
git clone https://github.com/JoshuaTurner3/simple-ethernet-sim.git
cd simple-ethernet-sim

# To build
cmake -B build .
cd build
make .

# To run the tests
./build/tests

# To run the demo
./build/ethernet_demo
```

## Request for Discussion (RFD)

### Summary

Implement a four-layer Ethernet simulation that demonstrates command / response,
telemetry streaming, and explicit error frames.

### Motivation

This project was created for a take-home embedded software software development challenge.

### Goals

The problem statement explicitly states:
- Implement basic Ethernet frame structure and handling.
- Create a simulated Ethernet driver (construct/parse frames, basic error checking).
- Implement a simple protocol on top of Ethernet.
- Use standard libraries if needed, but core Ethernet and protocol logic should be your own.
- Demonstrate:
    - Sending a command from "host" to "device"
    - Responding to the command
    - Handle streaming messages from "device"
    - Handling an error frame
- Put on Github & create a quickstart Readme in markdown format that instructs the reviewer how to set up and run as much of the prototype pipeline as you can get working, and how to run any test cases.
- As part of the Readme (after the quickstart), please include a Request for Discussion draft document explaining the approach. I recommend you complete this draft as the very first thing. It’s likely and expected that you won’t finish everything in the short period of this challenge; the draft will express all the aspects yet to be developed. (example of RFD’s here)

Additional goals I've added are:
- Modular, unit-testable code with strict layer boundaries
- Binary-accurate Ethernet frames (CRC-32, length rules)
- Zero dependencies beyond the standard library and Catch2
- Fault-injection hook for future fuzzing

### Non-goals

- Full IEEE 802.3 support (preamble, IPG, flow control, jumbo, VLAN)
- Persistence or retransmission (no ACK / NACK today)
- Hardware CRC offload (software CRC is fine for the sim)

### Architecture

Given the problem description, there are 4 network layers that will be implemented/used within this project.

| Layer           | File(s)                          | Notes                                             |
| --------------- | -------------------------------- | ------------------------------------------------- |
| Physical (mock) | `EthernetDriver.*`               | Two mutex-protected deques simulate TX/RX FIFOs   |
| Data link       | `EthernetFrame.*`                | Parses, serialises, and validates Ethernet frames |
| Network (sim)   | `EthernetDriver.*`               | `send` / `recv`, peer linking, error injection    |
| Transport / App | `Protocol.*` `Host.*` `Device.*` | 4-byte header, handlers for cmd/stream/error      |

### Protocol Design

The communication protocol consists of a primary `Msg` type defined as follows:

- Header (4 bytes)
    - `type`: 1 byte (see `MsgType`)
    - `id`: 1 byte (see `ID`)
    - `len`: 2 bytes little-endian payload length
- Data (Minimum 42 bytes)
    - If data length is less than `42` , then the packer adds leading zeroes to the final frame payload to satisfy the Ethernet minimum payload length

#### `MsgType`
| MsgType           | Direction      | Notes                                 |
| ----------------- | -------------  | ------------------------------------- |
| `COMMAND` (0x01)  | Host -> Device | Contains a `CmdID`.                   |
| `RESPONSE` (0x02) | Device -> Host | Echoes the `CmdID` that triggered it. |
| `STREAM` (0x03)   | Device ->Host  | Contains a `StreamID`.                |
| `ERROR` (0x04)    | Device ->Host  | Contains an `ErrorID`.                |

#### `ID`
ID is a union of the `CmdID`, `StreamID`, and `ErrorID` enums

##### `CmdID`
| CmdID                 | Payload | Response |
| --------------------- | ------- | ---------|
| `PING` (0x01)         | none    | `"PONG"` |
| `START_STREAM` (0x02) | none    | `"OK"`   |
| `STOP_STREAM` (0x03)  | none    | `"OK"`   |

##### `StreamID`
| StreamID           | Rate              | Payload layout               |
| ------------------ | ----------------- | ---------------------------- |
| `TELEMETRY` (0x01) | every device poll | 4-byte little-endian counter |

##### `ErrorID`
| ErrorID              | Meaning                  |
| -------------------- | ------------------------ |
| `UNKNOWN_CMD` (0x01) | Command not recognised   |
| `BAD_PAYLOAD` (0x02) | Length or format invalid |

### Implementation notes

- The CRC32 implementation uses the reversed polynomial so that emitted bytes match the endianess of those at the physical layer (big endian)
- The driver mutexes are coarse and solely for the purposes of simulating transmission via the queues
- The demo spawns 1 thread for the Device and uses the main thread to run the Host.

### Testing

| Scope                  | File               | Assertions |
| ---------------------- | ------------------ | ---------- |
| Frame encode / decode  | `TestFrame.cpp`    | 18         |
| Driver queue logic     | `TestDriver.cpp`   | 8          |
| Protocol pack / unpack | `TestProtocol.cpp` | 6          |
| Host <-> Device flow   | `TestProtocol.cpp` | 4          |

See the [quickstart](#quickstart) guide for how to run tests.

### Drawbacks

- Busy-wait loop in `Demo` is CPU-hungry. A condition variable would fix this.
- Length field is host-order to avoid byte-swap on little-endian MCUs, which breaks capture readability on big-endian hosts.

### Future Work

- ACK/NACK and retransmission window
- VLAN tag parsing
