#pragma once

#include <Arduino.h>

// Pin numbers live in config/pin.h only -- keeping them here too caused
// the previous duplication (RS485_EN_PIN here vs RS485_EN in pin.h).
namespace config {
    constexpr uint32_t UART_BAUD = 9600;
    constexpr uint32_t REQUEST_INTERVAL_MS = 40;
    constexpr size_t LINE_BUFFER_SIZE = 64;
    constexpr uint32_t RS485_TURNAROUND_US = 2000;
    constexpr uint8_t POLL_NODE_IDS[] = {1, 2, 3, 4};
    constexpr size_t POLL_NODE_COUNT = sizeof(POLL_NODE_IDS) / sizeof(POLL_NODE_IDS[0]);
    constexpr uint8_t MAX_RETRIES = 3;
    constexpr uint32_t RESPONSE_TIMEOUT_MS = 200;

    constexpr uint8_t DISPLAY_WIDTH = 128;
    constexpr uint8_t DISPLAY_HEIGHT = 64;
    constexpr uint8_t DISPLAY_I2C_ADDRESS = 0x3C;
}