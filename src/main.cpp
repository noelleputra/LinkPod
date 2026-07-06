#include <Arduino.h>
#include <cctype>
#include <cstring>
#include "config/config.h"
#include "communication/rs485.h"

namespace {
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
}  // namespace

Rs485 rs485;

void setup() {
  Serial.begin(config::UART_BAUD);
  Serial.println("LinkPod ready");

  rs485.begin(config::UART_BAUD, config::RS485_EN_PIN);
  delay(100);
}

void loop() {
  static uint32_t lastRequestMs = 0;
  static char line[config::LINE_BUFFER_SIZE];

  const uint32_t now = millis();
  if (now - lastRequestMs >= config::REQUEST_INTERVAL_MS) {
    lastRequestMs = now;
    rs485.sendRequest(config::NODE_ID);
    Serial.print("Request sent: R");
    Serial.println(config::NODE_ID);
  }

  if (rs485.readResponse(line, sizeof(line))) {
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

  delay(50);
}
