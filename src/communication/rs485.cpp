#include <algorithm>
#include <cstring>

#include <RTClib.h>
#include "communication/rs485.h"
#include "config/config.h"
#include "config/pin.h"
#include "config/protocol.h"

extern RTC_PCF8563 rtc;

void Rs485::begin(uint32_t baud, uint8_t enPin) {
    this->enPin = enPin;
    pinMode(enPin, OUTPUT);
    setReceiveMode();
    Serial1.begin(baud, SERIAL_8N1, pin::RS485_RX_PIN, pin::RS485_TX_PIN);
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

    // 1. Get the current time object
    DateTime now = rtc.now();

    // 2. Format it into an ASCII string (Example format: HHMMSS)
    char timeStr[7];
    snprintf(timeStr, sizeof(timeStr), "%02d%02d%02d", now.hour(), now.minute(), now.second());

    const char nodeIdDigit = static_cast<char>('0' + targetNodeId);

    setTransmitMode();
    
    // 3. Write the formatted string bytes
    Serial1.write(timeStr); 
    
    Serial1.write(protocol::REQUEST[0]);
    Serial1.write(nodeIdDigit);
    Serial1.write('\r');
    Serial1.write('\n');
    Serial1.flush();
    setReceiveMode();
}

bool Rs485::readResponse(char* buffer, size_t bufferSize) {
    // Local (stack) buffer, NOT static: every call is its own fresh
    // request/response cycle. Keeping this static previously meant a
    // partial line left over from a timed-out call could get glued onto
    // the next call's bytes, producing corrupted-looking readings.
    char line[config::LINE_BUFFER_SIZE];
    size_t index = 0;

    const uint32_t start = millis();
    while (millis() - start < config::RESPONSE_TIMEOUT_MS) {
        while (Serial1.available()) {
            const char c = static_cast<char>(Serial1.read());
            if (c == '\r') {
                continue;
            }

            if (c == '\n') {
                if (index == 0) {
                    continue;
                }

                line[index] = '\0';

                if (std::strncmp(line, protocol::PREFIX, std::strlen(protocol::PREFIX)) != 0) {
                    index = 0;
                    continue;
                }

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
                index = 0; // oversized/malformed line, restart just this line
            }
        }

        delay(5);
    }

    return false;
}
