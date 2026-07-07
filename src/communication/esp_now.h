#pragma once

#include <Arduino.h>
#include <esp_now.h>

class EspNow {
public:
    void begin();
    void send(uint8_t nodeId, uint8_t soil1, uint8_t soil2);
};
