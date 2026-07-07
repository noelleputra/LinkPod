#include "app/gateway_service.h"

GatewayService gatewayService;

void setup()
{
    Serial.begin(9600);
    delay(500);
    Serial.println("LinkPod start");
    gatewayService.begin();
}

void loop()
{
    gatewayService.loop();
}