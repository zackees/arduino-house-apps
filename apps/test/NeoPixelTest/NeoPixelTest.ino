#include <Adafruit_NeoPixel.h>

#define PIN1 7
#define PIN2 6
#define N_PIXELS 48

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip0 = Adafruit_NeoPixel(N_PIXELS, PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(N_PIXELS, PIN2, NEO_GRB + NEO_KHZ800);


void setup() {
  Serial.begin(9600);
  
  strip0.begin();
  strip0.show(); // Initialize all pixels to 'off'
  strip1.begin();
  strip1.show(); // Initialize all pixels to 'off'
  //Serial.println("Done!");
}

void loop() {
  int r, g, b;
  r = g = b = 255;  // max power is 255.
  for (int i = 0; i < N_PIXELS; ++i) {
     strip0.setPixelColor(i, r, g, b);
     strip1.setPixelColor(i, r, g, b);
     strip0.show();
     strip1.show();
     delay(100);
     strip0.setPixelColor(i, 0, 0, 0);
     strip1.setPixelColor(i, 0, 0, 0);
  }
  Serial.println("Draw");
}
