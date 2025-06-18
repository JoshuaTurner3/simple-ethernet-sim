# Quickstart

# Request for Discussion (RFD)

## Context
This RFD outlines the design approach for the creation of a simple ethernet driver simulation for a time-boxed coding challenge. The following will be discussed in-order:

1. The Problem Statement
2. Goals for the Project
3. Program Architecture
4. Communication Protocol Design
5. Building and Testing
6. Concluding Discussion

## Problem Statement
The exact specifications provided by the team are:

```txt
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
```

Therefore, the project is essentially to implement the lowest layers of the network stack for an embedded device; that is, it must implement:

- Ethernet frame parsing and construction
- Ethernet frame validation (error checking)
- An ethernet driver for ease of use
- A simple communication protocol
- Error handling at all levels

## Goals

The provided problem statement gives some clearly set goals to strive for; however, the following goals should also be met:

- Code must be modular and testable
- There must be a clear separation of concerns between communication layers
- Must demonstraction basic communication
- Must have a simple method by which to intercept/inject raw frames for error testing and debugging

In addition to specifying goals, I think it is pertinent to state that this implementation will not:
- Be a real hardware Ethernet implementation
- Be a full implementation of the IEEE 802.3 specification (TODO: Add link)
- Implement protocol persistence or retransmission (TODO: check this after finishing)

## Program Architecture

Given the problem description, there are 4 network layers that will be implemented/used within this project.

1. **Physical Layer**: Simple implementation mocking hardware reception of electrical signals over the wire.
2. **Data Link Layer**: Will define the Ethernet frame structure, serialization, and parsing of data received from the physical layer.
3. **Network Layer**: Will simulate an ethernet interface to transmit raw bytes between endpoints.
4. **Transport Layer**: Will implement command and streaming behavior for a simple application test.

## Communication Protocol Design

TODO: Think of a good stream setup

## Building and Testing

TODO: Update at end

## Concluding Discussion

TODO: When finished/submitting

