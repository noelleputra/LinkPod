#include "drivers/sdcard.h"
#include <FS.h>
#include <SD.h>
#include <SPI.h>

static constexpr char LOG_FILENAME[] = "/data.csv";

// One dedicated SPI bus instance for the SD card so it does not
// conflict with any other SPI peripherals on the same board.
static SPIClass sdSpi(FSPI);

void SdCard::begin(uint8_t csPin,
                   uint8_t misoPin, uint8_t mosiPin, uint8_t sckPin,
                   Pcf8563* rtc)
{
    this->csPin = csPin;
    this->rtc   = rtc;

    // Initialise SPI with the explicit pin mapping for ESP32-C3 Super Mini.
    // Calling SPI.begin() without arguments uses SDK default pins which
    // may differ across board definitions and cause silent failures.
    sdSpi.begin(sckPin, misoPin, mosiPin, csPin);

    if (!SD.begin(csPin, sdSpi)) {
        Serial.println("SD card init failed (no card / wiring issue)");
        ready = false;
        return;
    }

    ready = true;
    Serial.printf("SD card ready (%llu MB)\n",
                  SD.cardSize() / (1024ULL * 1024ULL));
    ensureHeader();
}

void SdCard::ensureHeader()
{
    // Only write the header when the file is brand-new so reboots
    // do not insert duplicate header rows mid-log.
    if (SD.exists(LOG_FILENAME)) {
        File f = SD.open(LOG_FILENAME, FILE_READ);
        if (f && f.size() > 0) {
            f.close();
            return;
        }
        if (f) f.close();
    }

    File f = SD.open(LOG_FILENAME, FILE_WRITE);
    if (f) {
        f.println("date,time,node_id,soil1_pct,soil2_pct,soil3_pct,soil4_pct");
        f.close();
    } else {
        Serial.println("SD: failed to write CSV header");
    }
}

void SdCard::log(uint8_t nodeId,
                 uint8_t soil1, uint8_t soil2,
                 uint8_t soil3, uint8_t soil4)
{
    if (!ready) return;

    // FILE_APPEND: opens or creates the file and seeks to EOF
    // before every write.
    File f = SD.open(LOG_FILENAME, FILE_APPEND);
    if (!f) {
        Serial.println("SD: failed to open /data.csv for append");
        return;
    }

    if (rtc != nullptr) {
        const DateTime t = rtc->now();
        f.printf("%04d-%02d-%02d,%02d:%02d:%02d,%u,%u,%u,%u,%u\n",
                 t.year(), t.month(), t.day(),
                 t.hour(), t.minute(), t.second(),
                 nodeId, soil1, soil2, soil3, soil4);
    } else {
        // No RTC -- use millis() as a relative timestamp.
        f.printf("%lu,,%u,%u,%u,%u,%u\n",
                 millis(), nodeId, soil1, soil2, soil3, soil4);
    }

    f.close();
}
