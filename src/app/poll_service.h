#pragma once

#include "communication/rs485.h"
#include "config/config.h"

// Last-known status of one polled node, kept around across poll cycles
// so the display can keep showing it (and how stale it is) even while
// other nodes are being polled or a node is temporarily unreachable.
struct NodeStatus
{
    uint8_t nodeId = 0;
    uint8_t soil1 = 0;
    uint8_t soil2 = 0;
    bool everSucceeded = false;
    unsigned long lastSuccessMs = 0;
};

class PollService
{
public:
    void begin();
    bool update();

    uint8_t nodeId() const;
    uint8_t soil1() const;
    uint8_t soil2() const;

    // Per-node last-known status, indexed 0..POLL_NODE_COUNT-1 (same
    // order as config::POLL_NODE_IDS) -- for the display's persistent
    // multi-node view.
    const NodeStatus& statusAt(uint8_t index) const;
    static uint8_t nodeCount();

    // True once the bus has failed so persistently that it's more
    // likely broken than just having one flaky node -- see
    // config::COMMS_UNHEALTHY_SKIP_STREAK.
    bool commsUnhealthy() const;

private:
    bool parseResponse(const char* buffer, uint8_t expectedNodeId);
    void advanceToNextNode();

    Rs485 rs485;
    uint8_t currentNodeIndex = 0;
    uint8_t retryCount = 0;
    uint8_t lastPolledNodeId = 0;
    uint8_t soil1Value = 0;
    uint8_t soil2Value = 0;
    unsigned long lastPollAtMs = 0;

    NodeStatus nodeStatuses[config::POLL_NODE_COUNT];
    uint16_t consecutiveSkipStreak = 0;
};
