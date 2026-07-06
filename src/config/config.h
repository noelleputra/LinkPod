#pragma once

#include <Arduino.h>

namespace config {
    constexpr uint8_t NODE_ID = 1;
    constexpr uint32_t UART_BAUD = 9600;
    constexpr uint32_t REQUEST_INTERVAL_MS = 500;
    constexpr size_t LINE_BUFFER_SIZE = 64;
    constexpr uint8_t RS485_EN_PIN = 4;
    constexpr uint8_t RS485_RX_PIN = 20;
    constexpr uint8_t RS485_TX_PIN = 21;
    constexpr uint32_t RS485_TURNAROUND_US = 200;
}