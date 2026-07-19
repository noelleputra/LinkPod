#include "drivers/display.h"

bool Display::begin()
{
    if (!oled.begin(SSD1306_SWITCHCAPVCC, config::DISPLAY_I2C_ADDRESS)) {
        Serial.println("Display init failed");
        return false;
    }

    oled.setTextColor(SSD1306_WHITE);
    oled.setTextSize(1);
    showStatus("LinkPod ready");
    return true;
}

void Display::showStatus(const char* line)
{
    oled.clearDisplay();
    oled.setCursor(0, 0);
    oled.print(line);
    oled.display();
}

void Display::showReading(uint8_t nodeId, uint8_t soil1, uint8_t soil2, uint8_t soil3, uint8_t soil4)
{
    oled.clearDisplay();
    oled.setCursor(0, 0);
    oled.printf("Node  %u\nSoil1 %u%%\nSoil2 %u%%\nSoil3 %u%%\nSoil4 %u%%", nodeId, soil1, soil2, soil3, soil4);
    oled.display();
}

void Display::showNodes(const NodeStatus* statuses, uint8_t count, bool commsUnhealthy, const char* timeText)
{
    oled.clearDisplay();
    oled.setCursor(0, 0);

    // Time header: shown on every refresh (top line) whenever the RTC
    // has produced a value. Kept out of the commsUnhealthy branch too,
    // since knowing the current time is still useful while the bus is
    // down.
    if (timeText != nullptr && timeText[0] != '\0') {
        oled.println(timeText);
    }

    if (commsUnhealthy) {
        // Bus-level failure takes over the whole screen -- per-node
        // detail doesn't matter if RS485 itself looks dead.
        oled.println("RS485 ERROR");
        oled.println("no nodes reachable");
        oled.println("check bus / wiring");
        oled.display();
        return;
    }

    const unsigned long now = millis();

    for (uint8_t i = 0; i < count; ++i) {
        const NodeStatus& s = statuses[i];

        if (!s.everSucceeded) {
            oled.printf("N%u  -- -- -- --  ERR\n", s.nodeId);
            continue;
        }

        const unsigned long age = now - s.lastSuccessMs;
        if (age > config::NODE_STALE_MS) {
            // Had good data before, just not recently -- show the last
            // known values plus how old they are rather than blanking
            // them out, but flag it clearly as stale.
            oled.printf("N%u %3u%3u%3u%3u%% OLD %lus\n", s.nodeId, s.soil1, s.soil2, s.soil3, s.soil4, age / 1000);
        } else {
            oled.printf("N%u %3u%3u%3u%3u%%\n", s.nodeId, s.soil1, s.soil2, s.soil3, s.soil4);
        }
    }

    oled.display();
}
