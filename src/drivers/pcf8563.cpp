#include "drivers/pcf8563.h"

bool Pcf8563::begin()
{
    if (!rtc.begin()) {
        Serial.println("RTC init failed");
        return false;
    }
    return true;
}

void Pcf8563::formatTime(char* buffer, size_t bufferSize)
{
    const DateTime now = rtc.now();
    snprintf(buffer, bufferSize, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
}
