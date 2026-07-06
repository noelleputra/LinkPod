#include <Arduino.h>
#include <cctype>
#include <cstring>
#include "config/config.h"


void sendRequestToSensorPod() {
  Serial1.print("R");
  Serial1.println(config::NODE_ID);
  Serial1.flush();
  Serial.print("Request sent: R");
  Serial.println(config::NODE_ID);
}

bool parseSensorResponse(const char* line, uint8_t& soil1, uint8_t& soil2) {
  const char* prefix = "SensorPod";
  if (std::strncmp(line, prefix, std::strlen(prefix)) != 0) {
    return false;
  }

  const char* cursor = line + std::strlen(prefix);
  while (*cursor != '\0' && std::isdigit(static_cast<unsigned char>(*cursor))) {
    ++cursor;
  }

  if (*cursor != ':') {
    return false;
  }

  char* end = nullptr;
  const long parsedSoil1 = std::strtol(cursor + 1, &end, 10);
  if (end == cursor + 1 || *end != ',') {
    return false;
  }

  const long parsedSoil2 = std::strtol(end + 1, &end, 10);
  if (end == cursor + 1 || *end != '\0') {
    return false;
  }

  soil1 = static_cast<uint8_t>(parsedSoil1);
  soil2 = static_cast<uint8_t>(parsedSoil2);
  return true;
}

void setup() {
  Serial.begin(config::UART_BAUD);
  Serial.println("LinkPod ready");

  pinMode(20, INPUT);
  pinMode(21, OUTPUT);
  Serial1.begin(config::UART_BAUD, SERIAL_8N1, 20, 21);
  delay(100);
}

void loop() {
  static uint32_t lastRequestMs = 0;
  static char line[config::LINE_BUFFER_SIZE];
  static size_t lineLength = 0;

  const uint32_t now = millis();
  if (now - lastRequestMs >= config::REQUEST_INTERVAL_MS) {
    lastRequestMs = now;
    sendRequestToSensorPod();
  }

  while (Serial1.available()) {
    const char c = static_cast<char>(Serial1.read());

    if (c == '\n') {
      line[lineLength] = '\0';

      if (lineLength > 0) {
        uint8_t soil1 = 0;
        uint8_t soil2 = 0;

        if (parseSensorResponse(line, soil1, soil2)) {
          Serial.print("Sensor data => S1:");
          Serial.print(soil1);
          Serial.print(" S2:");
          Serial.println(soil2);
        } else {
          Serial.print("Raw response: ");
          Serial.println(line);
        }
      }

      lineLength = 0;
    } else if (c != '\r') {
      if (lineLength < config::LINE_BUFFER_SIZE - 1) {
        line[lineLength++] = c;
      }
    }
  }

  delay(50);
}
