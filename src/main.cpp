#include <Wire.h>
#include "app/gateway_service.h"
#include "config/pin.h"
#include "drivers/pcf8563.h"

Pcf8563 rtc;
GatewayService gatewayService;

void setup()
{
    Serial.begin(9600);
    delay(500);

    // I2C must be up before anything on the bus (RTC, OLED) inits.
    Wire.begin(pin::SDA_PIN, pin::SCL_PIN);
    rtc.begin();

    Serial.println("LinkPod start");
    gatewayService.begin();
}

void loop()
{
    gatewayService.loop();
}
