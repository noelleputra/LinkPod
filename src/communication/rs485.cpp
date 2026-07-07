#include <algorithm>
#include <cstring>

#include "communication/rs485.h"
#include "config/protocol.h"

void Rs485::begin(uint32_t baud, uint8_t enPin) {
    this->enPin = enPin;
    pinMode(enPin, OUTPUT);
    setReceiveMode();
    Serial1.begin(baud, SERIAL_8N1, pin::RS485_RX, pin::RS485_TX);
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
    while (Serial1.available()) {
        Serial1.read();
    }

    const char nodeIdDigit = static_cast<char>('0' + targetNodeId);

    Serial.printf("RS485 tx %c%c\r\n", protocol::REQUEST[0], nodeIdDigit);

    setTransmitMode();
    delayMicroseconds(1000);
    Serial1.write(protocol::REQUEST[0]);
    Serial1.write(nodeIdDigit);
    Serial1.write('\r');
    Serial1.write('\n');
    Serial1.flush();
    delayMicroseconds(1000);
    setReceiveMode();
    delay(50);
}

bool Rs485::readResponse(char* buffer, size_t bufferSize) {
    static char line[config::LINE_BUFFER_SIZE];
    static size_t index = 0;

    const uint32_t start = millis();
    while (millis() - start < config::RESPONSE_TIMEOUT_MS) {
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

                if (std::strncmp(line, protocol::PREFIX, std::strlen(protocol::PREFIX)) != 0) {
                    continue;
                }

                if (buffer != nullptr && bufferSize > 0) {
                    const size_t len = std::min<size_t>(bufferSize - 1, std::strlen(line));
                    std::memcpy(buffer, line, len);
                    buffer[len] = '\0';
                }

                if (buffer != nullptr && buffer[0] != '\0') {
                    Serial.printf("RS485 rx %s\n", buffer);
                }
                return true;
            }

            if (index < (config::LINE_BUFFER_SIZE - 1)) {
                line[index++] = c;
            } else {
                index = 0;
            }
        }

        delay(5);
    }

    return false;
}
