#pragma once

#include "app/poll_service.h"
#include "communication/esp_now.h"
#include "drivers/display.h"
#include "drivers/pcf8563.h"

class GatewayService
{
public:
    // rtc is owned by main.cpp (it must be begin()'d after Wire.begin(),
    // before anything else touches the I2C bus) and passed in by
    // reference so GatewayService can read the time each display
    // refresh without owning/initializing the RTC itself.
    void begin(Pcf8563* rtc);
    void loop();

private:
    PollService pollService;
    EspNow espNow;
    Display display;
    Pcf8563* rtc = nullptr;
    bool ledState = false;
    unsigned long lastDisplayRefreshMs = 0;
};
