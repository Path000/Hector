#include "Ecran.h"

Adafruit_SSD1306 display(OLED_RESET);

void Ecran::init() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  clear();
}

void Ecran::refresh() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.setTextSize(2);
  for(byte i=0; i<LINES; i++) {
    display.println(displayedBuffer[i]);
  }
  display.display();

}

void Ecran::set(byte ligne, String text) {
  displayedBuffer[constrain(ligne, 0, LINES-1)] = text;
}

void Ecran::clear() {
  for(byte i=0; i<LINES; i++) {
    set(i, "");
  }
  refresh();
}
