# ESP32 Wake Over MQTT
Wake your PC anywhere using MQTT and ESP32.

## Description
This project utilize browser javascript to send MQTT wake resuests, 
then ESP32 can listen to this message and wake PC using magic packet. 

We use EMQX Cloud (Free Tier) as the MQTT provider, Github Pages as the web hosting, then ESP32 as the client. As the result, this could be an very cheap solution for wake over WAN.

## Features
### 💡 Low energy costs
The only device that comsumption power is the ESP32 at home. So you won't need to worry about the electricity bill even if ESP32 runs all day.

### 💰 Low devices costs 
By using the cheap ESP32 micro controller and free services on the internet, this project only requires very low budget.

### ￹￹￹￹￹🌐 Safety and privacy
The advantage of MQTT is that you don't need to expose your IP to host an local website. And the https protocol can protect the login information compared to HTTP server on ESP32.

### 🚀 High stability
ESP32 runs faster and more stable on MQTT listener compared to an HTTP server, this might be an better solution of the wake-on-wan scenario. 

## Installation

### Part A: MQTT Server

This part we use [EMQX Cloud](https://www.emqx.com/en) as the MQTT server provider. You can also find other services you desired or self-hosted server if you preferred.

After creating the server, you will need to write the information below for futher steps:

| Name | Description | 
| :--- | :-----      |
| Broker | The IP and port of the MQTT server  |
| Topic  | The topic for ESP32 to listen and web appliction to send requests |

### Part B: ESP32 Micro Controller

First, clone this repo to your computer, and edit the ``config.h`` in the root directory.

#### Instructions
| Variable | Description | Type |
| :---     | :---        | :--- |
| WIFI_SSID | The name of the WiFi that ESP32 connect | char[] |
| WIFI_PSK  | The password of the WiFi that ESP32 connect | char[] |
| MQTT_BROKER | The domain of the MQTT server | char[] |
| MQTT_PORT | The port of the MQTT sevrer | int |
| MQTT_USER | Username to login to the MQTT server | char[] |
| MQTT_PASS | Password to login with the username | char[] |
| MQTT_TOPIC | MQTT topic that ESP32 need to subscribe | char[] |
| MQTT_CLIENT_ID | The client ID of the ESP32 | char[] |
| OLED_DISPLAY | Should the OLED display be used | int |
| LOG_LEVEL | Set log level for serial port debugging | 0: ERROR <br> 1: WARN <br> 2: INFO <br> 3: DEBUG | 
| LOG_TIMESTAMP | Put timestamp on the log message | int | 
| CA_CERT | Certificate to connect to the MQTT server <br> (Comment the section if not used) | char[] |
| RESTART_INTERVAL | ESP32 restart interval in miliseconds <br> (Set value to -1 to prevent auto restart ) | int |
| devices[] | A list of device object | List\<Device> |

#### An example of the Device object
```cpp
Device(
  IPAddress(192, 168, 1, 1), // IPv4 address of the target device
  "xx:xx:xx:xx:xx:xx", // MAC address in lower case
  "MY-PC", // Device name that shows on website
  "assets/Windows11.png", // Image url to display on the web
  "Description here", // Device description shown on the website
  4 // LED pin to indicate the device is running or not (-1 to disable)
)
```
### Part C: The website

Since the website is static, this project utilize Github Pages to host.

First you will need to prepare the IP address of your MQTT server over websocket. (e.g. ``wss://a1234567.ala.us-east-1.emqxsl.com``) And then navigate to ``Settings`` > ``Secrets and variables`` on github, and add two variables called ``MQTT_BROKER`` and ``MQTT_PORT`` by pressing the ``New repository secret`` button.

Navigate to the ``Actions`` tab on Github, and then click on ``Deploy to GitHub Pages`` workflow. Afterward, run it and wait until it complete. 

Finally, enable the Github Page function in the repo settings and set ``web/`` as the root directory, then the whole setup is done.