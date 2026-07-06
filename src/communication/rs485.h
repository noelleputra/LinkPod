#pragma once

#include <Arduino.h>
#include "config/config.h"

class Rs485 {
public:
    void begin(uint32_t baud, uint8_t enPin = config::RS485_EN_PIN);
    void sendRequest(uint8_t targetNodeId);
    bool readResponse(char* buffer, size_t bufferSize);

private:
    void setTransmitMode();
    void setReceiveMode();

    uint8_t enPin = config::RS485_EN_PIN;
};
