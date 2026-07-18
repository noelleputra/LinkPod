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

    // A node's last known reading is shown as "stale" on the OLED once
    // it's older than this, even if the value itself is still cached.
    constexpr uint32_t NODE_STALE_MS = 5000;

    // Comms-health watchdog: counts consecutive times a node was skipped
    // after exhausting MAX_RETRIES with zero successes anywhere in
    // between. If every node is dead at once this climbs fast; once it
    // crosses this many skips (~5 full rotations of 4 nodes), the RS485
    // bus itself is assumed stuck and the gateway restarts itself rather
    // than sitting there polling a dead bus forever.
    constexpr uint16_t COMMS_UNHEALTHY_SKIP_STREAK = 20;

    // loop() spins as fast as it can; redrawing the OLED on every single
    // tick would hammer the I2C bus for no visible benefit. This paces
    // the persistent status refresh independently of the poll cadence.
    constexpr uint32_t DISPLAY_REFRESH_INTERVAL_MS = 250;

    constexpr uint8_t DISPLAY_WIDTH = 128;
    constexpr uint8_t DISPLAY_HEIGHT = 64;
    constexpr uint8_t DISPLAY_I2C_ADDRESS = 0x3C;
}