#pragma once

#include <Adafruit_SSD1306.h>
#include "config/config.h"

// Thin wrapper around Adafruit_SSD1306, scoped to the one thing LinkPod
// needs to show: which sensorpod node was last relayed and its readings.
class Display
{
public:
    bool begin();
    void showStatus(const char* line);
    void showReading(uint8_t nodeId, uint8_t soil1, uint8_t soil2);

private:
    Adafruit_SSD1306 oled{config::DISPLAY_WIDTH, config::DISPLAY_HEIGHT, &Wire, -1};
};
