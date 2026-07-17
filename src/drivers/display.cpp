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

void Display::showReading(uint8_t nodeId, uint8_t soil1, uint8_t soil2)
{
    oled.clearDisplay();
    oled.setCursor(0, 0);
    oled.printf("Node  %u\nSoil1 %u%%\nSoil2 %u%%", nodeId, soil1, soil2);
    oled.display();
}
