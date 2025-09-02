class MQTT {
  public:
    MQTT(PubSubClient *mqttClient) {
      _mqttClient = mqttClient;
    }

    bool initialize(void) {
      _mqttClient->setServer(MQTT_BROKER, MQTT_PORT);
      _mqttClient->setKeepAlive(60);
      _mqttClient->setCallback(mqttCallback);

      connectToMQTT();
      return true;
    }

    void connectToMQTT() {
      while (!_mqttClient->connected()) {
        Serial.printf("Connecting to MQTT Broker as %s.....\n", MQTT_CLIENT_ID);

        if (_mqttClient->connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS)) {
          Serial.println("Connected to MQTT broker");
          
          _mqttClient->subscribe(MQTT_TOPIC);
          // Publish message upon successful connection
          _mqttClient->publish(MQTT_TOPIC, "Hi EMQX I'm ESP32 ^^"); 
        } else {
          Serial.print("Failed, rc=");
          Serial.print(_mqttClient->state());
          Serial.println(" try again in 5 seconds");
          delay(5000);
      }
    }
  }

  static void mqttCallback(char *mqtt_topic, byte *payload, unsigned int length) {
    Serial.print("Message received on mqtt_topic: ");
    Serial.println(mqtt_topic);
    Serial.print("Message: ");
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println("\n-----------------------");
  }

  void loop() {
    if (! _mqttClient->connected() ) {
      LOGE("MQTT", "Server connection lost, reconnecting...");
      connectToMQTT();
    }

    _mqttClient->loop(); 
  }
  
  private:
    PubSubClient *_mqttClient;
};