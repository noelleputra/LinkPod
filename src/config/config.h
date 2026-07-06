#pragma once

#include <Arduino.h>

namespace config {
    constexpr uint8_t NODE_ID = 1;
    constexpr uint32_t UART_BAUD = 9600;
    constexpr uint32_t REQUEST_INTERVAL_MS = 500;
    constexpr size_t LINE_BUFFER_SIZE = 64;
}