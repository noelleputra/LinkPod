#pragma once

#include <Arduino.h>

namespace config {
    constexpr uint8_t NODE_ID = 1;
    constexpr uint32_t UART_BAUD = 9600;
    constexpr uint32_t REQUEST_INTERVAL_MS = 40;
    constexpr size_t LINE_BUFFER_SIZE = 64;
    constexpr uint8_t RS485_EN_PIN = 4;
    constexpr uint8_t RS485_RX_PIN = 20;
    constexpr uint8_t RS485_TX_PIN = 21;
    constexpr uint32_t RS485_TURNAROUND_US = 1000;
    constexpr uint8_t POLL_NODE_IDS[] = {1, 2, 3, 4};
    constexpr size_t POLL_NODE_COUNT = sizeof(POLL_NODE_IDS) / sizeof(POLL_NODE_IDS[0]);
    constexpr uint8_t MAX_RETRIES = 3;
    constexpr uint32_t RESPONSE_TIMEOUT_MS = 300;
}