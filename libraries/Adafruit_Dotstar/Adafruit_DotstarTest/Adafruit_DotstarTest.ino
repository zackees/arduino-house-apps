#define SPI_CLOCK 13
#define SPI_DATA 11

#include "SPI.h"
//#include "Intel_DotStar.h"

#include "Adafruit_DotStar.h"


Adafruit_DotStar strip = Adafruit_DotStar(1000, SPI_DATA, SPI_CLOCK, DOTSTAR_BGR);


void setup() {
  // put your setup code here, to run once:

  strip.begin();
  strip.show();
}

void SetAll(Adafruit_DotStar* strip, char r, char b, char g) {
  for (int i = 0; i < strip->numPixels(); ++i) {
    strip->setPixelColor(i, r, g, b);
  }
}

bool toggle = false;
void loop() {
  // put your main code here, to run repeatedly:

  toggle = !toggle;
  for (int i = 0; i < strip.numPixels(); ++i) {
    if (i % 2 == toggle) {
      strip.setPixelColor(i, 32, 0, 0);
    } else {
      strip.setPixelColor(i, 0, 32, 0);
    }
  }

  strip.show();
  delay(15);
}
