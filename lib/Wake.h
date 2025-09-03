class Device {
  public: 
    IPAddress ip;
    char mac[18];
    bool status = false;
    String title, image, lore;
    unsigned int lastPingTime;
    int pin;

  Device(IPAddress ip, char mac[18], String title, String image, String lore, int pin) {
    this->ip = ip;
    strcpy(this->mac, mac);
    this->title = title;
    this->image = image;
    this->lore = lore;
    this->pin = pin;
  };

  void initialize() {
    if (pin < 0) 
      return;

    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }

  bool ping() {
    if (millis() - lastPingTime < updateInterval*1000) 
      return status;

    lastPingTime = millis();
    return status = ping_start(ip, 1, 1, 32, 100);
  }

  void wake() {
    Log.verboseln("Sending magic packet to %s", title);
    WOL.sendMagicPacket(mac);
    return;
  }

  String toJson() {
    int strLen = title.length() + image.length() + lore.length() + 1;
    strLen += 2 + 9*4; // First 2 is for two brackets, and every row contains at least 9 characters for basic structure 

    char buffer[strLen];
    std::snprintf(
      buffer,
      strLen,
      "{\"na\":\"%s\", \"ip\":\"%s\", \"im\":\"%s\", \"st\": \"%d\"}",
      title.c_str(), lore.c_str(), image.c_str(), status
    );

    return buffer;
  }

  private:
    static const unsigned updateInterval = 20;
};
