#include "Arduino.h"


typedef void (*VisFunction)(unsigned long time_ms, int i, Gfx_Strip* strip);

// Explore this function to see how to write to the graphics array.
void TestVis();


// Paints a rainbow on the strips using the current time.
void FillWithRainbow(unsigned long time_ms, int i, Gfx_Strip* strip);

// Paints the whole thing white, useful for "closing time"
void FillWithWhite(unsigned long time_ms, int i, Gfx_Strip* strip);
void FillWithRed(unsigned long time_ms,   int i, Gfx_Strip* strip);
void FillWithGreen(unsigned long time_ms, int i, Gfx_Strip* strip);
void FillWithBlue(unsigned long time_ms, int i, Gfx_Strip* strip);


// Noise function
void FillWithNoiseRedBlue(unsigned long time_ms, int i , Gfx_Strip* strip);
void FillWithNoiseGreenBlue(unsigned long time_ms, int i , Gfx_Strip* strip);
void FillWithNoiseRainbow(unsigned long time_ms, int i , Gfx_Strip* strip);

// "Sound Equalizer FX"
void ApplyVolumeFX(uint8_t vol, int which_aud_channel, Gfx_Strip* strip);


////////////////////////////////////////////////////////////////////////////////////
/// 
/// Test Impl
/// 
////////////////////////////////////////////////////////////////////////////////////

void TestWriteAllPixels() {
  for (int white_pixel = 0; white_pixel < N_PIXELS_PER_STRIP; ++white_pixel) {
    Gfx_Begin();

    for (int which_strip = 0; which_strip < N_STRIPS; ++which_strip) {
      for (int pixel = 0; pixel < N_PIXELS_PER_STRIP; ++pixel) {
        Gfx_SetPixel(which_strip, pixel, (white_pixel == pixel) ? 0xffffffff : 0x0);
      }
    }
    delay(5);
    Gfx_End();
  }
}

void TestWriteAllStrips() {
  for (int white_pixel = 0; white_pixel < N_PIXELS_PER_STRIP; ++white_pixel) {
    Gfx_Begin();
    for (int strip_idx = 0; strip_idx < N_STRIPS; ++strip_idx) {
      Gfx_Strip strip;
      for (int pixel = 0; pixel < N_PIXELS_PER_STRIP; ++pixel) {
        strip.pixels[pixel] = (white_pixel == pixel) ? Gfx_UnpackColor(0xffffffff) : Gfx_UnpackColor(0);
      }
      Gfx_SetStrip(strip_idx, strip);
    }
    Gfx_End();
    delay(5);
  }
}


void TestVis() {
  Serial.println("TestVis: begin");
  TestWriteAllPixels();
  TestWriteAllStrips();
  Serial.println("TestVis: end");
}


////////////////////////////////////////////////////////////////////////////////////
/// 
/// Rainbow Visualizer
/// 
////////////////////////////////////////////////////////////////////////////////////


// phaseShift is the shift between each row.  phaseShift=0
// causes all rows to show the same colors moving together.
// phaseShift=180 causes each row to be the opposite colors
// as the previous.
//
// cycleTime is the number of milliseconds to shift through
// the entire 360 degrees of the color wheel:
// Red -> Orange -> Yellow -> Green -> Blue -> Violet -> Red
//
void rainbow(int phaseShift);
void TestRainbowVis();


int rainbowColors[180];

//// --- IMPL --- ////

unsigned int h2rgb(unsigned int v1, unsigned int v2, unsigned int hue)
{
  if (hue < 60) return v1 * 60 + (v2 - v1) * hue;
  if (hue < 180) return v2 * 60;
  if (hue < 240) return v1 * 60 + (v2 - v1) * (240 - hue);
  return v1 * 60;
}

void rainbow(int phaseShift)
{


  int color_hue = (millis() / 32ul) % 180ul;

  Gfx_Begin();
  for (int pixel = 0; pixel < N_PIXELS_PER_STRIP; pixel++) {
    for (int strip = 0; strip < N_STRIPS; strip++) {
      int index = (color_hue + pixel + strip*phaseShift/2) % 180;
      int color = rainbowColors[index];
      Gfx_SetPixel(strip, pixel, color);
    }
  }
  Gfx_End();  // draws.
}

// Convert HSL (Hue, Saturation, Lightness) to RGB (Red, Green, Blue)
//
//   hue:        0 to 359 - position on the color wheel, 0=red, 60=orange,
//                            120=yellow, 180=green, 240=blue, 300=violet
//
//   saturation: 0 to 100 - how bright or dull the color, 100=full, 0=gray
//
//   lightness:  0 to 100 - how light the color is, 100=white, 50=color, 0=black
//
int makeColor(unsigned int hue, unsigned int saturation, unsigned int lightness)
{
  unsigned int red, green, blue;
  unsigned int var1, var2;

  if (hue > 359) hue = hue % 360;
  if (saturation > 100) saturation = 100;
  if (lightness > 100) lightness = 100;

  // algorithm from: http://www.easyrgb.com/index.php?X=MATH&H=19#text19
  if (saturation == 0) {
    red = green = blue = lightness * 255 / 100;
  } else {
    if (lightness < 50) {
      var2 = lightness * (100 + saturation);
    } else {
      var2 = ((lightness + saturation) * 100) - (saturation * lightness);
    }
    var1 = lightness * 200 - var2;
    red = h2rgb(var1, var2, (hue < 240) ? hue + 120 : hue - 240) * 255 / 600000;
    green = h2rgb(var1, var2, hue) * 255 / 600000;
    blue = h2rgb(var1, var2, (hue >= 120) ? hue - 120 : hue + 240) * 255 / 600000;
  }
  return (red << 16) | (green << 8) | blue;
}

void RainbowVis_SetupOnce() {
  static bool initialized = false;
  if (initialized) return;

  for (int i=0; i<180; i++) {
    int hue = i * 2;
    int saturation = 100;
    int lightness = 50;
    // pre-compute the 180 rainbow colors
    rainbowColors[i] = makeColor(hue, saturation, lightness);
  }

  initialized = true;
}



void TestRainbowVis() {
  RainbowVis_SetupOnce();
  unsigned long time_now = millis();

  Gfx_Begin();
  for (int i = 0; i < N_STRIPS; ++i) {
    Gfx_Strip strip;
    FillWithRainbow(time_now, i, &strip);
    Gfx_SetStrip(i, strip);
  }
  Gfx_End();
}

void FillWithRainbow(unsigned long time_ms, int i, Gfx_Strip* strip) {
  PERF_TIMER;
  RainbowVis_SetupOnce();
  time_ms /= 32ul;
  time_ms %= 180ul;
  int color_hue = int(time_ms);
  
  for (int pixel = 0; pixel < N_PIXELS_PER_STRIP; pixel++) {
    int index = (color_hue + pixel) % 180;
    int color = rainbowColors[index];
    strip->pixels[pixel] = Gfx_UnpackColor(color);
  }
}


////////////////////////////////////////////////////////////////////////////////////
/// 
/// Fill with White Gradients Impl
/// 
////////////////////////////////////////////////////////////////////////////////////

void FillWithColor(Gfx_Strip* strip, uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < N_PIXELS_PER_STRIP; ++i) {
    Gfx_Pixel p = { r, g, b };
    strip->pixels[i] = p;  // White pixels.
  }
}

void FillWithWhite(unsigned long time_ms, int i, Gfx_Strip* strip) {
  FillWithColor(strip, 0xff, 0xff, 0xff);
}

void FillWithRed(unsigned long time_ms,   int i, Gfx_Strip* strip) {
  FillWithColor(strip, 0xff, 0x00, 0x00);
}
void FillWithGreen(unsigned long time_ms, int i, Gfx_Strip* strip) {
  FillWithColor(strip, 0x00, 0xff, 0x00);
}
void FillWithBlue(unsigned long time_ms, int i, Gfx_Strip* strip) {
  FillWithColor(strip, 0x00, 0x00, 0xff);
}


////////////////////////////////////////////////////////////////////////////////////
/// 
/// Fill with White Gradients Impl
/// 
////////////////////////////////////////////////////////////////////////////////////


struct NoiseGenerator {

  NoiseGenerator() {
    iteration_scale = 500;
    time_multiplier = 14;
  }

  NoiseGenerator (int32_t itScale, int32_t timeMul) : iteration_scale(itScale), time_multiplier(timeMul) {
  }
  // i is the position of the LED
  uint8_t Value(int32_t i, unsigned long time_ms) {
    uint32_t input = iteration_scale*i + time_ms * time_multiplier;
     
    uint16_t v1 = inoise16(input);
    return uint8_t(v1 >> 8);
  }

  int LedValue(int32_t i, unsigned long time_ms) {
    int val = Value(i, time_ms);
    val = max(0, val - 128) * 2;
    return Gfx_Gamma(uint8_t(val));
  }

  int32_t iteration_scale;
  unsigned long time_multiplier;
};

void FillWithNoiseRedBlue(unsigned long time_ms, int i , Gfx_Strip* strip) {
  NoiseGenerator red_noise(1000, 43);
  NoiseGenerator blue_noise (1000, 30);
  for (int i = 0; i < N_PIXELS_PER_STRIP; ++i) {
    Gfx_Pixel gp = { red_noise.LedValue(i, time_ms),
                     0,
                     blue_noise.LedValue(i, time_ms) };
    strip->pixels[i] = gp;
  }
}


void FillWithNoiseGreenBlue(unsigned long time_ms, int i , Gfx_Strip* strip) {
  NoiseGenerator green_noise(1000, 43);
  NoiseGenerator blue_noise (1000, 30);
  for (int i = 0; i < N_PIXELS_PER_STRIP; ++i) {
    Gfx_Pixel gp = { 0,
                     green_noise.LedValue(i, time_ms),
                     blue_noise.LedValue(i, time_ms) };
    strip->pixels[i] = gp;
  }
}


void FillWithNoiseRainbow(unsigned long time_ms, int i , Gfx_Strip* strip) {
  NoiseGenerator noise (2000, 100);
  FillWithRainbow(time_ms, i, strip);
  for (int i = 0; i < N_PIXELS_PER_STRIP; ++i) {
    uint8_t noiseVal = noise.LedValue(i, time_ms);
    Gfx_Pixel& p = strip->pixels[i];
    int tmp = p.r;
    tmp *= noiseVal;
    tmp /= 255;
    p.r = tmp;

    tmp = p.g;
    tmp *= noiseVal;
    tmp /= 255;
    p.g = tmp;

    tmp = p.b;
    tmp *= noiseVal;
    tmp /= 255;
    p.b = tmp;
  }
}


////////////////////////////////////////////////////////////////////////////////////
/// 
/// Volume Gradients Impl
/// 
////////////////////////////////////////////////////////////////////////////////////


struct VelocityPosition {
  VelocityPosition(int maxVal) : mPos(0), mMaxVal(maxVal) {}
  
  float Update(float new_pos, float scale) {
    float deltaPos = new_pos - mPos;
    bool signPositive = (deltaPos > 0.0f);

    // Hack that gets this function to work better.
    if (new_pos == 0) { deltaPos = -20; }
    else if (fabs(deltaPos) < 20) { deltaPos /= 10.0; }

    float vel = deltaPos * scale;
    
    if (signPositive) { // attack
      vel *= 4.0f;      // attack speed.
      if (mPos + vel > new_pos) { mPos = new_pos; }
      else { mPos += vel; }
    } else {            // decay
      vel *= .4;        // decay speed.
      if (mPos + vel < new_pos) { mPos = new_pos; }
      else { mPos += vel; }
    }

    if (mPos > mMaxVal) { mPos = mMaxVal; }
    if (mPos < 0.0f)    { mPos = 0.0f; }
    return mPos;
  }

  float mPos;
  const float mMaxVal;
};

VelocityPosition velocityPosition[N_AUDIO_CHANNELS] = { VelocityPosition(255), VelocityPosition(255), VelocityPosition(255), VelocityPosition(255) };

void ApplyVolumeFX(uint8_t vol, int which_aud_channel, Gfx_Strip* strip) {
  PERF_TIMER;

  if (which_aud_channel >= N_AUDIO_CHANNELS || which_aud_channel < 0) {
    SP(__FUNCTION__); SP(": audio channel "); SP(which_aud_channel); SPLN(" is out of bounds");
    return;
  }

  vol = (uint8_t)velocityPosition[which_aud_channel].Update(vol, 0.1);

  //audioFilters[which_aud_channel].Update(vol);
  //int midPoint = audioFilters[which_aud_channel].Position();
  int midPoint = vol * N_PIXELS_PER_STRIP / N_PIXELS_PER_STRIP;

  //SP(__FUNCTION__); SP(": "); SPLN(midPoint);

 
  
  for (int i = 0; i < N_PIXELS_PER_STRIP; ++i) {
    // If below midpoint then blacken pixel.
    if (i >= midPoint) {
      strip->pixels[i] = Gfx_UnpackColor(0x00);
    }
  }
}

