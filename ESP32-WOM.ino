#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <ArduinoLog.h>
#include <ESP32Ping.h>
#include <WakeOnLan.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

// Wake on lan requirement
WiFiUDP UDP;
WakeOnLan WOL(UDP);

#include "lib/Wake.h"
#define DEVICE_COUNT sizeof(devices) / sizeof(Device)

#include "config.h"
#include "assets/Icons.h"
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

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

void mqttCallback(char*, byte*, unsigned int);

void setup() {
  Serial.begin(115200); // Enable Serial Port Logging
  Log.begin   (6, &Serial);

  connection = new Connection(WIFI_SSID, WIFI_PSK, &oled);
  mqttClient = new PubSubClient(espClient);
  mqttHandler = new MQTT(mqttClient, mqttCallback);

  Log.verboseln ("System started.");

  if (! oled.begin() ) {
    Log.fatal("OLED display failed, ESP32 will restart soon...");
    delay(1000);
    esp_restart();
  }

  Log.verboseln ("OLED display initialized.");

  if (! connection->initialize() ) {
    Log.fatal("WiFi connection failed, ESP32 will restart soon...");
    delay(1000);
    esp_restart();
  }

  Log.verboseln ("WiFi connection initialized.");

  // Set Root CA certificate
  #ifdef CA_CERT
  espClient.setCACert(CA_CERT);
  Log.verboseln ("Connection is secured by SSL.");
  #endif
  
  // Initialize MQTT server connection
  if (! mqttHandler->initialize() ) {
    Log.fatal("MQTT connection failed, ESP32 will restart soon...");
    delay(1000);
    esp_restart();
  }

  xTaskCreatePinnedToCore(ping_loop, "Ping", 6144, NULL, 1, NULL, ARDUINO_RUNNING_CORE);
}

void loop() {
  connection->loop();
  mqttHandler->loop();
}

void ping_loop(void *params) {
  unsigned int i = 0;
  while (i++ || 1) {
    devices[i % DEVICE_COUNT].ping();
    delay(100);
  }
}

void mqttCallback(char *mqtt_topic, byte *payload, unsigned int length) {
  String jsonString;
  for (unsigned int i = 0; i < length; i++) {
    jsonString += (char)payload[i];
  }

  Log.verboseln("Message received: %s", jsonString.c_str());

  StaticJsonDocument<256> doc;  // buffer 大小依你的 JSON 複雜度調整
  DeserializationError error = deserializeJson(doc, jsonString);

  if (error) {
    Log.error("JSON parse failed: %s", error.c_str());
    return;
  }

  if (!doc.containsKey("cmd")) {
    Log.error("JSON doesn't contains cmd key, skipping\n");
    return;
  }

  String cmd = doc["cmd"].as<String>();

  bool ok;
  if (cmd == "query") {
    ok = mqttHandler->client->publish(MQTT_TOPIC, String(DEVICE_COUNT).c_str());
    Serial.println(ok ? "Publish OK" : "Publish failed: unknown reason (buffer full?)");

    for (int i=0; i<DEVICE_COUNT; i++) {
      ok = mqttHandler->client->publish(MQTT_TOPIC, devices[i].toJson().c_str());
      Serial.println(ok ? "Publish OK" : "Publish failed: unknown reason (buffer full?)");
    }
    
  } else if (cmd == "wake") {
    if (!doc.containsKey("dev")) {
      Log.error("JSON doesn't contains dev key, abort waking device\n");
      return;
    }

    int device = doc["dev"].as<int>();

    if (device < 0 || device >= DEVICE_COUNT) {
      Log.error("Device id: %d out of range, abort waking device\n", device);
      return;
    }

    devices[device].wake();

    ok = mqttHandler->client->publish(MQTT_TOPIC, "OK");
    Serial.println(ok ? "Publish OK" : "Publish failed: unknown reason (buffer full?)");
  }
}