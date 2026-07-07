#include <cstring>
#include <WiFi.h>

#include "communication/esp_now.h"
#include "config/protocol.h"

namespace {
constexpr uint8_t kBroadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
}

void EspNow::begin() {
    Serial.println("Initializing ESP-NOW...");
    WiFi.mode(WIFI_MODE_STA);
    WiFi.disconnect();

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW init failed");
        return;
    }

    esp_now_peer_info_t peerInfo{};
    std::memcpy(peerInfo.peer_addr, kBroadcastAddress, sizeof(kBroadcastAddress));
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("ESP-NOW peer add failed");
        return;
    }

    Serial.println("ESP-NOW ready");
}

void EspNow::send(uint8_t nodeId, uint8_t soil1, uint8_t soil2) {
    protocol::SensorPacket packet{};
    packet.nodeId = nodeId;
    packet.soil1 = soil1;
    packet.soil2 = soil2;
    packet.timestamp = millis();

    Serial.printf("Sending ESP-NOW packet: node=%u soil1=%u soil2=%u\n", nodeId, soil1, soil2);

    const esp_err_t result = esp_now_send(const_cast<uint8_t *>(kBroadcastAddress), reinterpret_cast<uint8_t *>(&packet), sizeof(packet));
    if (result != ESP_OK) {
        Serial.printf("ESP-NOW send failed: %d\n", result);
    }
}
