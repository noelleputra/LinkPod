#include "app/gateway_service.h"
#include <RTClib.h>
#include "config/pin.h"

RTC_PCF8563 rtc;

GatewayService gatewayService;

void setup()
{
    Serial.begin(9600);
    delay(500);
    rtc.begin();
    Serial.println("LinkPod start");
    gatewayService.begin();
        if (!Wire.begin(pin::SDA_PIN, pin::SCL_PIN)) {
        Serial.println("Failed to initialize I2C bus hardware!");
    }
}


void loop()
{
    gatewayService.loop();
}