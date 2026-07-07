#pragma once

#include "communication/rs485.h"

class PollService
{
public:
    void begin();
    bool update();
    uint8_t nodeId() const;
    uint8_t soil1() const;
    uint8_t soil2() const;

private:
    Rs485 rs485;
    uint8_t currentNodeIndex = 0;
    uint8_t lastPolledNodeId = 0;
    uint8_t soil1Value = 0;
    uint8_t soil2Value = 0;
    unsigned long lastPollAtMs = 0;
};