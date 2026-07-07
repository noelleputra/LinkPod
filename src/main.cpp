#include <Arduino.h>
#include <cctype>
#include <cstring>
#include "config/config.h"
#include "communication/rs485.h"

namespace {
bool parseSensorResponse(const char* line, uint8_t& nodeId, uint8_t& soil1, uint8_t& soil2) {
  const char* prefix = "SP";
  if (std::strncmp(line, prefix, std::strlen(prefix)) != 0) {
    return false;
  }

  const char* cursor = line + std::strlen(prefix);
  if (*cursor == '\0' || !std::isdigit(static_cast<unsigned char>(*cursor))) {
    return false;
  }

  char* end = nullptr;
  const long parsedNodeId = std::strtol(cursor, &end, 10);
  if (end == cursor || *end != ':') {
    return false;
  }

  const char* soil1Start = end + 1;
  const long parsedSoil1 = std::strtol(soil1Start, &end, 10);
  if (end == soil1Start || *end != ',') {
    return false;
  }

  const char* soil2Start = end + 1;
  const long parsedSoil2 = std::strtol(soil2Start, &end, 10);
  if (end == soil2Start || *end != '\0') {
    return false;
  }

  nodeId = static_cast<uint8_t>(parsedNodeId);
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
  static size_t pollIndex = 0;
  static char line[config::LINE_BUFFER_SIZE];

  const uint32_t now = millis();
  if (now - lastRequestMs >= config::REQUEST_INTERVAL_MS) {
    lastRequestMs = now;

    const uint8_t targetNodeId = config::POLL_NODE_IDS[pollIndex];
    pollIndex = (pollIndex + 1) % config::POLL_NODE_COUNT;

    rs485.sendRequest(targetNodeId);
    Serial.print("Polling SensorPod R");
    Serial.println(targetNodeId);
  }

  if (rs485.readResponse(line, sizeof(line))) {
    uint8_t nodeId = 0;
    uint8_t soil1 = 0;
    uint8_t soil2 = 0;

    if (parseSensorResponse(line, nodeId, soil1, soil2)) {
      Serial.print("Sensor data from node ");
      Serial.print(nodeId);
      Serial.print(" => S1:");
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
