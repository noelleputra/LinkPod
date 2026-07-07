#pragma once

#include <Arduino.h>
#include "config/config.h"
#include "config/pin.h"

class Rs485 {
public:
    void begin(uint32_t baud = config::UART_BAUD, uint8_t enPin = pin::RS485_EN);
    void sendRequest(uint8_t targetNodeId);
    bool readResponse(char* buffer, size_t bufferSize);

private:
    void setTransmitMode();
    void setReceiveMode();
    uint8_t enPin = pin::RS485_EN;
    uint8_t dirPin = 0xFF;
};
