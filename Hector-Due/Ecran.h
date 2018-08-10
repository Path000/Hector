#ifndef Ecran_h
#define Ecran_h

#include <Wire.h>
#include <Adafruit_SSD1306.h>

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


#define NB_LINES 8



class Ecran {
  public:
  	Adafruit_SSD1306 display;
    void init();
    void clear();
    void set(byte ligne, String text);
    void refresh();
    void pixel(uint8_t x, uint8_t y);
  private:
    String displayedBuffer[NB_LINES];
};


#endif
