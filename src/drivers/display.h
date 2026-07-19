#pragma once

#include <Adafruit_SSD1306.h>
#include "config/config.h"
#include "app/poll_service.h"

// Thin wrapper around Adafruit_SSD1306, scoped to what LinkPod needs to
// show: every known node's last reading, refreshed every loop tick so a
// node that stops responding visibly goes stale/errors out on screen
// instead of the display just freezing on old data.
class Display
{
public:
    bool begin();
    void showStatus(const char* line);
    void showReading(
        uint8_t nodeId, 
        uint8_t soil1, 
        uint8_t soil2,
        uint8_t soil3,
        uint8_t soil4
    
    );

    // Persistent view: one line per node, showing its last known
    // reading plus how old it is. Meant to be called every loop tick
    // (not just on a successful poll) so a dead node visibly goes
    // stale/ERR instead of the screen just freezing on old numbers.
    // timeText is an optional pre-formatted "HH:MM:SS" string (pass
    // nullptr or "" to omit it, e.g. before the RTC has been read yet).
    void showNodes(const NodeStatus* statuses, uint8_t count, bool commsUnhealthy, const char* timeText = nullptr);

private:
    Adafruit_SSD1306 oled{config::DISPLAY_WIDTH, config::DISPLAY_HEIGHT, &Wire, -1};
};
