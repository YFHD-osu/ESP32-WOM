class Connection {
  public:
    Connection(char *ssid, char *pass, SSD1306 *oled) {
      _ssid = ssid;
      _pass = pass;
      _oled = oled;
    }

    bool initialize(void) {  
      LOGI("Wifi", "Connecting to %s", _ssid);

      return connect();
    }

    bool connect() {
      int timeCounter = 0;
      
      WiFi.begin(_ssid, _pass);

      while (WiFi.status() != WL_CONNECTED) {
        // Update every 250ms 
        delay(250);

        timeCounter ++;
        
        _oled->drawWIFI(timeCounter%2 ? gImage_wifi_connecting : gImage_wifi_blank);
        if (timeCounter > timeout * 4) break;
      }
      
      if (WiFi.status() != WL_CONNECTED) {
        LOGE("Wifi", "Failed to connect to %s", _ssid);
        _oled->drawWIFI(gImage_wifi_failure);
        return false;
      }

      LOGI("Wifi", "Connect success.\n Local IP:%s\n Subnet Mask: %s", WiFi.localIP().toString(), WiFi.subnetMask().toString());
      _oled->drawWIFI(gImage_wifi_connected);

      return true;
    }

    void loop() {
      if (WiFi.status() != WL_CONNECTED) {
        LOGE("WiFi", "Connection lost, reconnecting...");
        connect();
      }

      return;
    }
  
  private:  
    char *_ssid, *_pass;
    SSD1306 *_oled;

    // Wifi connect timeout in second
    const int timeout = 20;
};

