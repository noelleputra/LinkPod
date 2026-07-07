#include "app/poll_service.h"
#include <cstdlib>
#include <cstring>

#include "config/config.h"
#include "config/protocol.h"

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
    const bool gotLine = rs485.readResponse(buffer, sizeof(buffer));
    const bool parsedOk = gotLine && parseResponse(buffer, targetNodeId);

    if (parsedOk) {
        // One line per successful poll -- enough to see soil1-4 rotating
        // through without flooding the monitor.
        Serial.printf("[N%u] OK soil1=%u soil2=%u\n", targetNodeId, soil1Value, soil2Value);
        lastPolledNodeId = targetNodeId;
        advanceToNextNode();
        return true;
    }

    Serial.printf("[N%u] %s (retry %u/%u)\n",
                  targetNodeId,
                  gotLine ? "bad response" : "timeout",
                  retryCount + 1,
                  config::MAX_RETRIES);

    // Previously nothing advanced currentNodeIndex on failure, so one
    // dead/disconnected node could starve nodes 2-4 forever. Now we give
    // it MAX_RETRIES attempts, then move on.
    ++retryCount;
    if (retryCount >= config::MAX_RETRIES) {
        Serial.printf("[N%u] no reply after %u tries, skipping\n", targetNodeId, config::MAX_RETRIES);
        advanceToNextNode();
    }
    return false;
}

void PollService::advanceToNextNode()
{
    retryCount = 0;
    currentNodeIndex = static_cast<uint8_t>((currentNodeIndex + 1) % config::POLL_NODE_COUNT);
}

bool PollService::parseResponse(const char* buffer, uint8_t expectedNodeId)
{
    const size_t prefixLen = std::strlen(protocol::PREFIX);
    if (std::strncmp(buffer, protocol::PREFIX, prefixLen) != 0) {
        return false;
    }

    const char* separator = std::strchr(buffer, protocol::RESPONSE_DELIMITER);
    if (separator == nullptr) {
        return false;
    }

    // The node id embedded between the prefix and ':' must match who we
    // actually asked -- guards against silently accepting a stale or
    // crossed response as if it came from the node we just polled.
    uint8_t respondedNodeId = 0;
    for (const char* p = buffer + prefixLen; p < separator; ++p) {
        if (*p < '0' || *p > '9') {
            return false;
        }
        respondedNodeId = static_cast<uint8_t>(respondedNodeId * 10 + (*p - '0'));
    }
    if (respondedNodeId != expectedNodeId) {
        return false;
    }

    const char* comma = std::strchr(separator + 1, protocol::FIELD_DELIMITER);
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
