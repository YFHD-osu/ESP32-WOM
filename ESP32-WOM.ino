#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <U8g2lib.h>

#include "config.h"

#include "assets/Icons.h"
#include "lib/Logger.h"
#include "lib/MQTT.h"
#include "lib/Oled.h"
#include "lib/Connection.h"

SSD1306 oled;
PubSubClient* mqttClient = nullptr; 
Connection* connection = nullptr;
MQTT* mqttHandler = nullptr;

#ifdef CA_CERT
WiFiClientSecure espClient;
#else
WiFiClient espClient;
#endif

void setup() {
  Serial.begin(115200); // Enable Serial Port Logging

  #ifdef CA_CERT
  // Set Root CA certificate
  const char* ca_cert = CA_CERT;
  espClient.setCACert(ca_cert);
  #endif

  connection = new Connection(WIFI_SSID, WIFI_PSK, &oled);
  mqttClient = new PubSubClient(espClient);
  mqttHandler = new MQTT(mqttClient);

  if (! oled.begin() ) {
    esp_restart();
  }

  if (! connection->initialize() ) {
    esp_restart();
  }

}

void loop() {
  connection->loop();
  mqttHandler->loop();
}