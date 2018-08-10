#include "Ecran.h"

void Ecran::init() {

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  clear();
}

void Ecran::refresh() {

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.setTextSize(1);
  for(byte i=0; i<NB_LINES; i++) {
    display.println(displayedBuffer[i]);
  }
  display.display();
}

void Ecran::set(byte ligne, String text) {

  displayedBuffer[constrain(ligne, 0, NB_LINES-1)] = text;
}

void Ecran::clear() {

  for(byte i=0; i<NB_LINES; i++) {
    set(i, "");
  }
  display.clearDisplay();
}

void Ecran::pixel(uint8_t x, uint8_t y) {

  display.drawPixel(x, y, WHITE);
  display.display();
}