#include "Arduino.h"

#define N_PIXELS_PER_STRIP 95
#define N_STRIPS 8
#define GFX_CONFIG (WS2811_GRB | WS2811_800kHz)

// No idea why this is necessary but apparently we have a strip that has
// a component order that isn't officially specified so we have to do additional
// bit shifting to get it right.
#define SWAP_GREEN_BLUE 1

struct Gfx_Pixel { uint8_t r, g, b; };
struct Gfx_Strip {
  Gfx_Pixel pixels[N_PIXELS_PER_STRIP];
  Gfx_Strip() { memset(pixels, 0, sizeof(pixels)); }
};

void Gfx_Setup();
void Gfx_Begin();
void Gfx_End();

uint8_t Gfx_GetBrightness();
void Gfx_SetBrightness(int new_brightness);

Gfx_Pixel Gfx_GetPixel(int which_strip, int pixel);
void Gfx_SetPixel(int which_strip, int i, Gfx_Pixel p);
void Gfx_SetStrip(int which_strip, const Gfx_Strip& strip);
Gfx_Pixel Gfx_UnpackColor(int color);

uint8_t Gfx_Gamma(uint8_t in);


//// --- IMPL --- ////

DMAMEM int displayMemory[N_PIXELS_PER_STRIP*6];
int drawingMemory[N_PIXELS_PER_STRIP*6];
OctoWS2811 leds(N_PIXELS_PER_STRIP, displayMemory, drawingMemory, GFX_CONFIG);

uint8_t s_brightness = 255;
uint8_t s_brightness_gamma = Gfx_Gamma(s_brightness);

static byte gamma_correct(uint8_t in) {
  static const uint8_t gamma_table[] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };
  
  
  return (byte)gamma_table[in];
}

uint8_t Gfx_GetBrightness() {
  return s_brightness;
}
void Gfx_SetBrightness(int new_brightness) {
  s_brightness = static_cast<uint8_t>(constrain(new_brightness, 0, 255));
  s_brightness_gamma = Gfx_Gamma(s_brightness);
}

uint8_t Gfx_Gamma(uint8_t in) {
  return gamma_correct(in);
}


void Gfx_Begin() {
  PERF_TIMER;
  pinMode(1, OUTPUT);
  digitalWrite(1, HIGH);
}

void Gfx_End() {
  PERF_TIMER;
  leds.show();
  digitalWrite(1, LOW);
  delayMicroseconds(1);
}

static int ToPixelIndex(int which_strip, int which_pixel) {
  return which_strip * N_PIXELS_PER_STRIP + which_pixel;
}

void Gfx_SetPixel(int which_strip, int i, int color) {
  Gfx_Pixel p = Gfx_UnpackColor(color);
  Gfx_SetPixel(which_strip, i, p);
}

void Gfx_SetPixel(int which_strip, int i, uint8_t r, uint8_t b, uint8_t g) {
  if (s_brightness_gamma != 255) {
    r = uint8_t(int(r) * s_brightness_gamma / 255);
    g = uint8_t(int(g) * s_brightness_gamma / 255);
    b = uint8_t(int(b) * s_brightness_gamma / 255);
  }
  if (SWAP_GREEN_BLUE) {
    uint8_t tmp = b;
    b = g;
    g = tmp;
  }
  leds.setPixel(ToPixelIndex(which_strip, i), r, g, b);
}

void Gfx_SetPixel(int which_strip, int i, Gfx_Pixel p) {
  Gfx_SetPixel(which_strip, i, p.r, p.g, p.b);
}

void Gfx_SetStrip(int which_strip, const Gfx_Strip& strip) {
  for (int i = 0; i < N_PIXELS_PER_STRIP; ++i) {
    Gfx_SetPixel(which_strip, i, strip.pixels[i]);
  }
}

Gfx_Pixel Gfx_GetPixel(int which_strip, int pixel) {
  int color = leds.getPixel(ToPixelIndex(which_strip, pixel));
  Gfx_Pixel out = Gfx_UnpackColor(color);
  return out;
}

void Gfx_Setup() {
  Gfx_Begin();
  Gfx_Pixel black = { 0, 0, 0 };
  for (int x = 0; x < N_PIXELS_PER_STRIP; x++) {
    for (int y = 0; y < 8; y++) {
      Gfx_SetPixel(x, y, black);
    }
  }
  Gfx_End();
  leds.begin();
}

Gfx_Pixel Gfx_UnpackColor(int color) {
  int r = (color >> 16); 
  int g = (color >> 8);  
  int b = (color >> 0);  

  r = r & 0xff;
  g = g & 0xff;
  b = b & 0xff;

  Gfx_Pixel out = { uint8_t(r), uint8_t(g), uint8_t(b) };
  
  return out;
}

