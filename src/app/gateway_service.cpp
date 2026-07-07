#include "app/gateway_service.h"

void GatewayService::begin()
{
    pollService.begin();
    espNow.begin();
}

void GatewayService::loop()
{
    if (!pollService.update()) {
        return;
    }

    espNow.send(
        pollService.nodeId(),
        pollService.soil1(),
        pollService.soil2());
}
