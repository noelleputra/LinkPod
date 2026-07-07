#pragma once

#include "app/poll_service.h"
#include "communication/esp_now.h"

class GatewayService
{
public:
    void begin();
    void loop();

private:
    PollService pollService;
    EspNow espNow;
    bool ledState = false;
};
