#pragma once

#include <Arduino.h>

namespace pin {
    constexpr uint8_t RS485_EN_PIN = 3;
    constexpr uint8_t RS485_RX_PIN = 20;
    constexpr uint8_t RS485_TX_PIN = 21;
    constexpr uint8_t LED_PIN = 5;
    constexpr uint8_t SDA_PIN = 8;
    constexpr uint8_t SCL_PIN = 9;
    constexpr uint8_t SD_CS_PIN  = 7;   // SD card chip-select
    // SPI bus wired to the SD card reader.
    // ESP32-C3 Super Mini hardware SPI2: MISO=6, MOSI=7, SCK=10.
    // These must be declared explicitly -- the SDK defaults are board-
    // dependent and not guaranteed to match this wiring.
    constexpr uint8_t SD_MISO_PIN = 5;
    constexpr uint8_t SD_MOSI_PIN = 6;
    constexpr uint8_t SD_SCK_PIN  = 4;
}
