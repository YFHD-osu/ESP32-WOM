#ifndef CONFIG_INCLUDE
#define CONFIG_INCLUDE

/* ==================================================================
|  WiFi Section                                                     |
|  - Fill in WiFi SSID and PSK for ESP32 to start the web server on |
================================================================== */
#define WIFI_SSID "WIFI"
#define WIFI_PSK  "PASS"

/* ========================================
|  MQTT Section                           |
|  - Set your MQTT broker and credential  |
======================================== */
#define MQTT_BROKER "MY.MQTT.SERVER.com"
#define MQTT_PORT   1234
#define MQTT_USER   "USERNAME"
#define MQTT_PASS   "PASSWORD"
#define MQTT_TOPIC  "esp32/custom-topic"
#define MQTT_CLIENT_ID "esp32-random-string"

/* =======================
|  OLED Section          |
|  - Enable OLED display |
======================= */
#define OLED_DISPLAY false

/* =======================================================
|  Logging Section                                       |
|  - Serial port logging rules                           |
|  - LOG_LEVEL: 0=>ERROR, 1=>WARNNING, 2=>INFO, 3=>DEBUG |
======================================================= */
#define LOG_LEVEL 1
#define LOG_TIMESTAMP false

/* ==============================================================================
|  CERTIFICATE Section                                                          |
|  - Enable TLS for MQTT (You can leave it blank if server doesn't support TLS) |
============================================================================== */
#define CA_CERT "-----BEGIN CERTIFICATE-----\n" \
  "ABCDE123...\n" \
  "......\n" \
  "-----END CERTIFICATE-----\n" 

/* ===================================
|  Auto restart time                 |
|  - Set ESP32 restart interval (ms) |
|  - Set value to -1 to disable      |
=================================== */
#define RESTART_INTERVAL 60*60*1000

/* ===================================================
|  Device Section                                    |
|  - Declare all your device needs to be wake on lan |
|  - Further information on README.md                |
=================================================== */
Device devices[] = {
  Device(IPAddress(192, 168, 1, 1), "xx:xx:xx:xx:xx:xx", "MY-PC", "assets/Windows11.png", "Description here", 4),
  Device(IPAddress(192, 168, 1, 2), "xx:xx:xx:xx:xx:xx", "MY-Laptop", "assets/WindowsServer.png", "Description here", 0),
  Device(IPAddress(192, 168, 1, 2), "xx:xx:xx:xx:xx:xx", "MY-iMac", "assets/MacOS.png", "Description here", 2),
};

#endif
