#ifndef OLED_INCLUDED
#define OLED_INCLUDED

#include <Wire.h>
#include <U8g2lib.h>

class u8g2: public U8G2_SSD1306_128X64_NONAME_F_HW_I2C {
  public: 
    u8g2(const u8g2_cb_t *rotation, uint8_t reset = U8X8_PIN_NONE, uint8_t clock = U8X8_PIN_NONE, uint8_t data = U8X8_PIN_NONE) :
      U8G2_SSD1306_128X64_NONAME_F_HW_I2C(rotation, reset, clock, data) {};
};

class SSD1306: public u8g2  {
  public:
    SSD1306() : u8g2(U8G2_R0, U8X8_PIN_NONE) {};

    bool initialize();
    void println(String context);
    void sendBuffer();
    void clearBuffer();
    void drawWIFI(const unsigned char*);
    void drawDDNS(const unsigned char*);
    void drawServer(const unsigned char*);
    void update(unsigned int);
    void clearArea(int, int, int, int);

  private:
    int yPos = 14;
    unsigned int lastUpdate = 0;
    static const int statsHeight = 48;
};

#if OLED_DISPLAY

bool SSD1306::initialize() {
  u8g2::begin();
  u8g2::enableUTF8Print();  // 啟動 UTF8 支援
  u8g2::setFontMode(0);
  u8g2::setDrawColor(1);
  u8g2::setFont(u8g2_font_unifont_t_chinese1); // 使用 chinese1字型檔
  drawWIFI(gImage_wifi_blank);
  drawServer(gImage_server_blank);
  drawDDNS(gImage_DDNS_disabled);
  update(-1);

  return true;
}

void SSD1306::println(String context) {
  u8g2::setCursor(0, yPos); //自動換行
  u8g2::print(context);
  yPos += 16;
  return;
}

void SSD1306::clearBuffer() { //清除暫存器內容
  yPos = 14;
  u8g2::clearBuffer();
  return;
}

void SSD1306::drawWIFI(const unsigned char* image) {
  u8g2::drawXBM(112, statsHeight, 16, 16, image);
  u8g2::sendBuffer();
}

void SSD1306::drawServer(const unsigned char* image) {
  u8g2::drawXBM(96, statsHeight, 16, 16, image);
  u8g2::sendBuffer();
}

void SSD1306::drawDDNS(const unsigned char* image) {
  u8g2::drawXBM(80, statsHeight, 16, 16, image);
  u8g2::sendBuffer();
}

void SSD1306::clearArea(int x, int y, int w, int h) {
  u8g2::setFontMode(1);
  u8g2::setDrawColor(0);
  u8g2::drawRBox(x, y, w, h, 0);
  u8g2::setFontMode(0);
  u8g2::setDrawColor(1);
}

void SSD1306::update(unsigned int interval) {
  const unsigned int now = millis();
  
  if (lastUpdate - now < interval && interval > 0) return;
  int day = now%(24*60*60*60*1000) / (24*60*60*1000);
  int hour = now%(60*60*60*1000) / (60*60*1000);
  int minute = now%(60*60*1000) / (60*1000);
  int second = now%(60*1000) / 1000;
  String formatNow = 
    String(day) + "d" + 
    String(hour) + "h" + 
    String(minute) + "m" + 
    String(second) + "s";
  clearArea(0, statsHeight, 80, 16);
  u8g2::setFont(u8g2_font_profont12_tf); // 使用 chinese1字型檔
  u8g2::drawStr(0, statsHeight+13, formatNow.c_str());
  u8g2::sendBuffer();
  u8g2::setFont(u8g2_font_unifont_t_chinese1); // 使用 chinese1字型檔
}

#else

bool SSD1306::initialize() {
  Serial.println("OLED module is disabled, skip the initialization");
  return true;
}

void SSD1306::println(String context) {
}

void SSD1306::clearBuffer() {
}

void SSD1306::drawWIFI(const unsigned char* image) {
}

void SSD1306::drawServer(const unsigned char* image) {
}

void SSD1306::drawDDNS(const unsigned char* image) {
}

void SSD1306::clearArea(int x, int y, int w, int h) {
}

void SSD1306::update(unsigned int interval) {
}

#endif


#endif
