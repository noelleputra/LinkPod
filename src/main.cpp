#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <cstring>

struct SensorPacket
{
  uint8_t nodeId;
  uint8_t soil1;
  uint8_t soil2;
  uint32_t timestamp;
};

static constexpr uint8_t kNodeId = 1;
static constexpr uint8_t kBroadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void onDataSent(const uint8_t *macAddr, esp_now_send_status_t status)
{
  Serial.print(F("ESP-NOW status: "));
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? F("OK") : F("FAILED"));
  (void)macAddr;
}

void sendSensorData(uint8_t soil1, uint8_t soil2)
{
  SensorPacket packet{};
  packet.nodeId = kNodeId;
  packet.soil1 = soil1;
  packet.soil2 = soil2;
  packet.timestamp = millis();

  esp_err_t result = esp_now_send(const_cast<uint8_t *>(kBroadcastAddress), reinterpret_cast<uint8_t *>(&packet), sizeof(packet));
  if (result != ESP_OK)
  {
    Serial.print(F("Gagal mengirim ESP-NOW: "));
    Serial.println(result);
  }
}

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8N1, 20, 21);
  pinMode(20, INPUT);
  pinMode(21, OUTPUT);
  delay(100);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK)
  {
    Serial.println(F("Gagal menginisialisasi ESP-NOW"));
    while (true)
    {
      delay(1000);
    }
  }

  esp_now_register_send_cb(onDataSent);

  esp_now_peer_info_t peerInfo{};
  std::memcpy(peerInfo.peer_addr, kBroadcastAddress, sizeof(kBroadcastAddress));
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println(F("Gagal menambahkan peer ESP-NOW"));
    while (true)
    {
      delay(1000);
    }
  }

  Serial.println(F("LinkPod siap. Menunggu data SensorPod..."));
}

void loop()
{
  static uint32_t lastRequest = 0;
  static String line;

  if (millis() - lastRequest >= 10000)
  {
    lastRequest = millis();
    Serial1.flush();
    Serial1.println(F("REQ"));
    Serial1.flush();
    Serial.println(F("Permintaan REQ terkirim ke SensorPod"));
  }

  while (Serial1.available())
  {
    const char c = static_cast<char>(Serial1.read());
    if (c == '\n')
    {
      line.trim();
      if (line.startsWith("SensorPod:"))
      {
        line.remove(0, 10);

        const int comma = line.indexOf(',');
        if (comma > 0)
        {
          const int soil1 = line.substring(0, comma).toInt();
          const int soil2 = line.substring(comma + 1).toInt();

          Serial.print(F("Data diterima: "));
          Serial.print(soil1);
          Serial.print(F(", "));
          Serial.println(soil2);

          sendSensorData(static_cast<uint8_t>(soil1), static_cast<uint8_t>(soil2));
        }
      }
      line.clear();
    }
    else if (c != '\r')
    {
      line += c;
    }
  }

  delay(50);
}