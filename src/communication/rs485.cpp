#include <algorithm>
#include <cstring>

#include "communication/rs485.h"

void Rs485::begin(uint32_t baud, uint8_t enPin) {
    this->enPin = enPin;
    pinMode(enPin, OUTPUT);
    setReceiveMode();
    Serial1.begin(baud, SERIAL_8N1, config::RS485_RX_PIN, config::RS485_TX_PIN);
}

void Rs485::setTransmitMode() {
    digitalWrite(enPin, HIGH);
    delayMicroseconds(config::RS485_TURNAROUND_US);
}

void Rs485::setReceiveMode() {
    digitalWrite(enPin, LOW);
    delayMicroseconds(config::RS485_TURNAROUND_US);
}

void Rs485::sendRequest(uint8_t targetNodeId) {
    setTransmitMode();
    Serial1.print("R");
    Serial1.println(targetNodeId);
    Serial1.flush();
    setReceiveMode();
}

bool Rs485::readResponse(char* buffer, size_t bufferSize) {
    static char line[config::LINE_BUFFER_SIZE];
    static size_t index = 0;

    while (Serial1.available()) {
        const char c = static_cast<char>(Serial1.read());
        if (c == '\r') {
            continue;
        }

        if (c == '\n') {
            if (index == 0) {
                index = 0;
                continue;
            }

            line[index] = '\0';
            index = 0;

            if (buffer != nullptr && bufferSize > 0) {
                const size_t len = std::min<size_t>(bufferSize - 1, std::strlen(line));
                std::memcpy(buffer, line, len);
                buffer[len] = '\0';
            }
            return true;
        }

        if (index < (config::LINE_BUFFER_SIZE - 1)) {
            line[index++] = c;
        } else {
            index = 0;
        }
    }

    return false;
}
