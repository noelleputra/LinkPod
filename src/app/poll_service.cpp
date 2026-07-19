#include "app/poll_service.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "config/config.h"
#include "config/protocol.h"

void PollService::begin()
{
    rs485.begin();

    for (uint8_t i = 0; i < config::POLL_NODE_COUNT; ++i) {
        nodeStatuses[i].nodeId = config::POLL_NODE_IDS[i];
    }
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
        Serial.printf("[N%u] OK soil1=%u soil2=%u\n soil3=%u\n soil4=%u\n", targetNodeId, soil1Value, soil2Value, soil3Value, soil4Value);
        lastPolledNodeId = targetNodeId;

        NodeStatus& status = nodeStatuses[currentNodeIndex];
        status.soil1 = soil1Value;
        status.soil2 = soil2Value;
        status.soil3 = soil3Value;
        status.soil4 = soil4Value;
        status.everSucceeded = true;
        status.lastSuccessMs = now;

        // Any real success means the bus is alive -- whatever run of
        // failures came before it is no longer meaningful.
        consecutiveSkipStreak = 0;

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
        if (consecutiveSkipStreak < UINT16_MAX) {
            ++consecutiveSkipStreak;
        }
        advanceToNextNode();
    }
    return false;
}

const NodeStatus& PollService::statusAt(uint8_t index) const
{
    return nodeStatuses[index];
}

uint8_t PollService::nodeCount()
{
    return config::POLL_NODE_COUNT;
}

bool PollService::commsUnhealthy() const
{
    return consecutiveSkipStreak >= config::COMMS_UNHEALTHY_SKIP_STREAK;
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

    // Wire format is "SPn:soil1,soil2,soil3,soil4" -- walk the three
    // FIELD_DELIMITER commas in sequence. The previous version searched
    // for a single comma and then re-read from that same position for
    // soil2, soil3 and soil4, so those three fields all ended up holding
    // (a truncated copy of) the same text instead of their own value.
    const char* field1Start = separator + 1;
    const char* comma1 = std::strchr(field1Start, protocol::FIELD_DELIMITER);
    if (comma1 == nullptr) {
        return false;
    }

    const char* field2Start = comma1 + 1;
    const char* comma2 = std::strchr(field2Start, protocol::FIELD_DELIMITER);
    if (comma2 == nullptr) {
        return false;
    }

    const char* field3Start = comma2 + 1;
    const char* comma3 = std::strchr(field3Start, protocol::FIELD_DELIMITER);
    if (comma3 == nullptr) {
        return false;
    }

    const char* field4Start = comma3 + 1;

    char soil1Text[8] = {0};
    char soil2Text[8] = {0};
    char soil3Text[8] = {0};
    char soil4Text[8] = {0};
    const size_t soil1Length = static_cast<size_t>(comma1 - field1Start);
    const size_t soil2Length = static_cast<size_t>(comma2 - field2Start);
    const size_t soil3Length = static_cast<size_t>(comma3 - field3Start);
    const size_t soil4Length = std::strlen(field4Start);

    std::strncpy(soil1Text, field1Start, std::min<size_t>(soil1Length, sizeof(soil1Text) - 1));
    std::strncpy(soil2Text, field2Start, std::min<size_t>(soil2Length, sizeof(soil2Text) - 1));
    std::strncpy(soil3Text, field3Start, std::min<size_t>(soil3Length, sizeof(soil3Text) - 1));
    std::strncpy(soil4Text, field4Start, std::min<size_t>(soil4Length, sizeof(soil4Text) - 1));

    soil1Value = static_cast<uint8_t>(std::atoi(soil1Text));
    soil2Value = static_cast<uint8_t>(std::atoi(soil2Text));
    soil3Value = static_cast<uint8_t>(std::atoi(soil3Text));
    soil4Value = static_cast<uint8_t>(std::atoi(soil4Text));
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

uint8_t PollService::soil3() const
{
    return soil3Value;
}

uint8_t PollService::soil4() const
{
    return soil4Value;
}