#ifndef Ecran_h
#define Ecran_h

#include <Wire.h>
#include <Adafruit_SSD1306.h>

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#define OLED_RESET 4

#define LINES 4

class Ecran {
  public:
    void init();
    void clear();
    void set(byte ligne, String text);
    void refresh();
  private:
    String displayedBuffer[LINES];
};


#endif
