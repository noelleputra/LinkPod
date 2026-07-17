#include "app/gateway_service.h"
#include "config/pin.h"

void GatewayService::begin()
{
    pinMode(pin::LED_PIN, OUTPUT);
    digitalWrite(pin::LED_PIN, LOW);

    pollService.begin();
    espNow.begin();
    display.begin();
}

void GatewayService::loop()
{
    if (!pollService.update()) {
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

    espNow.send(nodeId, soil1, soil2);
    display.showReading(nodeId, soil1, soil2);
}
