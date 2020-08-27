#include <Adafruit_NeoPixel.h>

#define PIN 7

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(48, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  //Serial.println("Done!");
}

void loop() {
  int r, g, b;
  r = g = b = 255;  // max power is 255.
  for (int i = 0; i < strip.numPixels(); ++i) {
     strip.setPixelColor(i, r, g, b);
     strip.show();
     delay(100);
     strip.setPixelColor(i, 0, 0, 0);
  }
  
  for (int i = 0; i < 256; ++i) {
    analogWrite(9, i);
    delay(1);
  }
  delay(250);
  for (int i = 0; i < 256; ++i) {
    analogWrite(9, 255 - i);
    delay(1);
  }
  
  
  analogWrite(9, 0);
  Serial.println("Draw");
}
