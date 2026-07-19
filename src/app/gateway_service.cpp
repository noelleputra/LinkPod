#include "app/gateway_service.h"
#include "config/pin.h"
#include "config/config.h"

void GatewayService::begin(Pcf8563* rtc)
{
    this->rtc = rtc;

    pinMode(pin::LED_PIN, OUTPUT);
    digitalWrite(pin::LED_PIN, LOW);

    pollService.begin();
    espNow.begin();
    display.begin();
}

void GatewayService::loop()
{
    const bool success = pollService.update();

    // Comms-health watchdog: if the bus has failed this persistently
    // across every node, it's more likely RS485 itself (wiring, EN
    // pin, a dead transceiver) than one bad sensor. Say so plainly and
    // restart instead of quietly polling a dead bus forever.
    char timeText[9] = {0};
    if (rtc != nullptr) {
        rtc->formatTime(timeText, sizeof(timeText));
    }

    if (pollService.commsUnhealthy()) {
        display.showNodes(nullptr, 0, true, timeText);
        Serial.println("RS485 bus looks dead (no node has responded in a long time), restarting...");
        delay(1000); // let the message actually reach the screen/serial before reset
        ESP.restart();
        return;
    }

    // Persistent status view: refreshed on its own cadence (not every
    // raw loop() tick, which would just hammer the I2C bus) so any node
    // that stops responding visibly goes stale/ERR on screen instead of
    // the display freezing on old numbers.
    const unsigned long now = millis();
    if (now - lastDisplayRefreshMs >= config::DISPLAY_REFRESH_INTERVAL_MS) {
        lastDisplayRefreshMs = now;

        NodeStatus rows[config::POLL_NODE_COUNT];
        for (uint8_t i = 0; i < config::POLL_NODE_COUNT; ++i) {
            rows[i] = pollService.statusAt(i);
        }
        display.showNodes(rows, config::POLL_NODE_COUNT, false, timeText);
    }

    if (!success) {
        return;
    }

    // Visual heartbeat: flips every time a poll succeeds and data is
    // forwarded over ESP-NOW, so you can see it's alive without the
    // serial monitor open.
    ledState = !ledState;
    digitalWrite(pin::LED_PIN, ledState ? HIGH : LOW);

    const uint8_t nodeId = pollService.nodeId();
    const uint8_t soil1 = pollService.soil1();
    const uint8_t soil2 = pollService.soil2();
    const uint8_t soil3 = pollService.soil3();
    const uint8_t soil4 = pollService.soil4();

    espNow.send(nodeId, soil1, soil2, soil3, soil4);
}
