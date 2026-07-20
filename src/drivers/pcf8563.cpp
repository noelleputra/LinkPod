#include "drivers/pcf8563.h"

bool Pcf8563::begin()
{
    if (!rtc.begin()) {
        Serial.println("RTC init failed");
        return false;
    }

    // PCF8563 sets a flag when it loses power (dead/missing backup
    // battery). In that state the stored time is meaningless -- it
    // defaults to 2000-01-01 00:00:00 which is why the display showed
    // "00:01:08" (just a few seconds of millis() drift from midnight).
    // Auto-set to compile time so the display is at least in the right
    // ballpark from first boot, even without a live time-sync source.
    if (rtc.lostPower()) {
        Serial.println("RTC lost power -- setting to compile time");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    return true;
}

void Pcf8563::adjust(const DateTime& dt)
{
    rtc.adjust(dt);
}

bool Pcf8563::lostPower()
{
    return rtc.lostPower();
}

DateTime Pcf8563::now()
{
    return rtc.now();
}

void Pcf8563::formatTime(char* buffer, size_t bufferSize)
{
    const DateTime t = rtc.now();
    snprintf(buffer, bufferSize, "%02d:%02d:%02d", t.hour(), t.minute(), t.second());
}
