#pragma once

#include <RTClib.h>

// Thin wrapper around RTC_PCF8563 so the rest of the codebase depends on
// our own type (mockable, single init point) instead of the library
// class directly.
class Pcf8563
{
public:
    // Initialises the RTC. If the chip reports it lost power (battery
    // dead / first run), the time is automatically set to the moment
    // this firmware was compiled so the display never shows 00:00:xx.
    // Call adjust() any time you want to sync to a known reference.
    bool begin();

    // Override the time explicitly (e.g. received over serial/MQTT).
    void adjust(const DateTime& dt);

    // Returns true if the chip's timekeeping was interrupted since the
    // last call to begin() or adjust() (CLKOUT stopped, VDD lost, etc.)
    bool lostPower();

    // Writes "HH:MM:SS" (8 chars, null-terminated) into buffer. buffer
    // must be at least 9 bytes.
    void formatTime(char* buffer, size_t bufferSize);

    // Returns the current DateTime from the RTC (for logging etc.).
    DateTime now();

private:
    RTC_PCF8563 rtc;
};
