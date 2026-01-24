class MQTT {
  public:
    PubSubClient *client;
    void (*callback) (char *, byte *, unsigned int);

    MQTT(PubSubClient *mqttClient, void (*callback) (char *, byte *, unsigned int) ) {
      client = mqttClient;
      this->callback = callback;
    }

    bool initialize(void) {
      client->setServer(MQTT_BROKER, MQTT_PORT);
      client->setKeepAlive(60);
      client->setCallback(callback);

      connectToMQTT();
      return true;
    }

    void connectToMQTT() {
      while (!client->connected()) {
        Log.verboseln ("Connecting to MQTT Broker as %s.....", MQTT_CLIENT_ID);

        if (client->connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS)) {
          Log.verboseln ("Connected to MQTT broker");

          // Publish message upon successful connection
          client->publish(MQTT_TOPIC, "Hi EMQX I'm ESP32 ^^"); 
          
          client->subscribe(MQTT_TOPIC);
        } else {
          Log.error("Failed, rc=%d, try again in 5 seconds...\n", client->state());
          delay(5000);
      }
    }
  }

  void loop() {
    if (! client->connected() ) {
      Log.notice("Server connection lost, reconnecting...");
      connectToMQTT();
    }

    client->loop(); 
  }
};