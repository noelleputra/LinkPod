#include "app/gateway_service.h"
#include "config/pin.h"

void GatewayService::begin()
{
    pinMode(pin::LED_PIN, OUTPUT);
    digitalWrite(pin::LED_PIN, LOW);

    pollService.begin();
    espNow.begin();
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

    espNow.send(
        pollService.nodeId(),
        pollService.soil1(),
        pollService.soil2());
}
