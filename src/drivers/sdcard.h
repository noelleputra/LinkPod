#pragma once

#include <Arduino.h>
#include <SPI.h>
#include "drivers/pcf8563.h"

// Thin driver around the ESP32 Arduino SD library.
// Logs one CSV row per successful node poll to "data.csv" on the card.
// If the card is absent or init fails, all log() calls are silently
// no-ops so the rest of the system keeps running normally.
//
// NOTE on GPIO 2 (CS pin): GPIO2 is a strapping pin on ESP32-C3.
// It must be HIGH during boot. If your SD module has a pull-down on CS
// and boot issues occur, add a 10 kΩ pull-up from GPIO2 to 3.3 V.
class SdCard
{
public:
    // csPin              -- chip-select GPIO (pin 2 on this board)
    // misoPin/mosiPin/sckPin -- SPI bus pins (explicit, because ESP32-C3
    //                       SDK defaults vary by board definition)
    // rtc                -- pointer to shared RTC (may be nullptr;
    //                       millis() is used for timestamps instead)
    void begin(uint8_t csPin,
               uint8_t misoPin, uint8_t mosiPin, uint8_t sckPin,
               Pcf8563* rtc = nullptr);

    // Append one data row for a node poll result.
    // CSV format:  YYYY-MM-DD,HH:MM:SS,nodeId,soil1,soil2,soil3,soil4
    void log(uint8_t nodeId,
             uint8_t soil1, uint8_t soil2,
             uint8_t soil3, uint8_t soil4);

    bool isReady() const { return ready; }

private:
    bool      ready  = false;
    Pcf8563*  rtc    = nullptr;
    uint8_t   csPin  = 0;
    SPIClass* spiObj = nullptr;

    void ensureHeader();
};
