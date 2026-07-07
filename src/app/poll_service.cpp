#include "app/poll_service.h"
#include <cstdlib>
#include <cstring>

#include "config/config.h"

void PollService::begin()
{
    rs485.begin();
}

bool PollService::update()
{
    const unsigned long now = millis();
    if ((now - lastPollAtMs) < config::REQUEST_INTERVAL_MS) {
        return false;
    }

    lastPollAtMs = now;

    const uint8_t targetNodeId = config::POLL_NODE_IDS[currentNodeIndex];
    rs485.sendRequest(targetNodeId);

    char buffer[64] = {0};
    if (!rs485.readResponse(buffer, sizeof(buffer))) {
        return false;
    }

    const char* prefix = "SP";
    if (std::strncmp(buffer, prefix, std::strlen(prefix)) != 0) {
        return false;
    }

    const char* separator = std::strchr(buffer, ':');
    if (separator == nullptr) {
        return false;
    }

    const char* comma = std::strchr(separator + 1, ',');
    if (comma == nullptr) {
        return false;
    }

    char soil1Text[8] = {0};
    char soil2Text[8] = {0};
    const size_t soil1Length = static_cast<size_t>(comma - (separator + 1));
    const size_t soil2Length = std::strlen(comma + 1);

    std::strncpy(soil1Text, separator + 1, std::min<size_t>(soil1Length, sizeof(soil1Text) - 1));
    std::strncpy(soil2Text, comma + 1, std::min<size_t>(soil2Length, sizeof(soil2Text) - 1));

    soil1Value = static_cast<uint8_t>(std::atoi(soil1Text));
    soil2Value = static_cast<uint8_t>(std::atoi(soil2Text));
    lastPolledNodeId = targetNodeId;
    currentNodeIndex = static_cast<uint8_t>((currentNodeIndex + 1) % config::POLL_NODE_COUNT);
    return true;
}

uint8_t PollService::nodeId() const
{
    return lastPolledNodeId;
}

uint8_t PollService::soil1() const
{
    return soil1Value;
}

uint8_t PollService::soil2() const
{
    return soil2Value;
}