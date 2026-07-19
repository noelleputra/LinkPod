#pragma once

#include <RTClib.h>

// Thin wrapper around RTC_PCF8563 so the rest of the codebase depends on
// our own type (mockable, single init point) instead of the library
// class directly.
class Pcf8563
{
public:
    bool begin();

    // Writes "HH:MM:SS" (8 chars, null-terminated) into buffer. buffer
    // must be at least 9 bytes.
    void formatTime(char* buffer, size_t bufferSize);

private:
    RTC_PCF8563 rtc;
};
