#pragma once

#include <Arduino.h>

namespace protocol {
    constexpr char REQUEST[] = "R";
    constexpr char PREFIX[] = "SP";
    constexpr char RESPONSE_DELIMITER = ':';
    constexpr char FIELD_DELIMITER = ',';

    struct SensorPacket {
        uint8_t nodeId;
        uint8_t soil1;
        uint8_t soil2;
        uint8_t soil3;
        uint8_t soil4;
        uint32_t timestamp;
    };

    constexpr uint8_t BROADCAST_ADDRESS[6] = {
        0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF
    };
}