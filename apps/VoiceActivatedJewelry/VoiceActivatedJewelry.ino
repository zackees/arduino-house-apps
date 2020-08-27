


// If this is defined then Serial.print(...) is disabled.
//#define ENABLE_LOW_POWER_SLEEP
#define TEST_LED_CIRCLE


#include "FHT.h" // include the library for power savings.
#include "FastAnalogRead.h"

#ifdef ENABLE_LOW_POWER_SLEEP
#include "LowPower.h"
#endif

#define MIC_PIN A5
#define DIGITAL_LED_PIN 11
#define INTENSITY_FACTOR 15
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strip = Adafruit_NeoPixel(12,               // n pixels
                                            DIGITAL_LED_PIN,  // pin
                                            NEO_GRB + NEO_KHZ800);
                                            
void LowPowerSleep() {
  #ifdef ENABLE_LOW_POWER_SLEEP
    LowPower.idle(SLEEP_30MS, ADC_OFF, TIMER4_OFF, TIMER3_OFF, TIMER1_OFF, 
                  TIMER0_OFF, SPI_OFF, USART1_OFF, TWI_OFF, USB_OFF);
  #else
    delay(30);
  #endif
}

float noise_threshold = 0;
void UpdateNoiseThreshold() {
  static unsigned long prev_millis = 0;
  unsigned long time_now = millis();
  if (time_now - prev_millis < 125) {
    return;  // Not time to run yet.
  }
  prev_millis = time_now;
  float vol = CalcVolume(MIC_PIN, 40);
  static const int N = 10;
  static float vol_history[N] = {0};
  static int pos = 0;
  
  vol_history[pos] = vol;
  
  pos = (pos + 1) % N;
  
  noise_threshold = vol_history[0];
  for (int i = 1; i < N; ++i) {
    noise_threshold += vol_history[i];
  }
  noise_threshold /= N;
  
  // Serial.println(noise_threshold * 1023);
}

void setup() {
  //Serial.begin(115200); // use the serial port
  Serial.begin(9600);
  pinMode(DIGITAL_LED_PIN, OUTPUT);
  digitalWrite(A4, HIGH);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  //EnableFastAnalogRead();  // Sample rate above voice activation.
}

float CalcVolume(int pin, int n_samples) {
  long abs_avg = 0;
  for (int i = 0; i < n_samples; ++i) {
    int sample = analogRead(pin);
    sample -= 511;
    if (sample < 0) {
      sample = -sample;
    }
    abs_avg += sample;
  }
  
  abs_avg /= n_samples;
  abs_avg = max(0, abs_avg - 10);  // Gets rid of noise.
  float vol = min(1.0f, abs_avg / 420.0f);

  return vol;
}

void DoVis(int brightness) {
 int r, g, b;
 for (int i = 0; i < strip.numPixels(); ++i) {
     unsigned long time_now = millis();
     double time_seconds = time_now / 1000.0;
     
     r = brightness * (sin(i*3 + time_seconds*6.0) * .5 + .5);
     //g = 32 * (sin(i + time_seconds*6.0) * .5 + .5);
     g = 0;
     b = brightness * (sin(i*5.0 + time_seconds*8.0) * .5 + .5);
  
     strip.setPixelColor(i, r, g, b);
  }
}


void loop() {
  UpdateNoiseThreshold();
  float vol = CalcVolume(MIC_PIN, 40);
  
  // Output to graph.
  //Serial.println(vol * 1023);
  
  static float intensity = 0;
  static float prev_intensity = 0;
  static unsigned long last_time = millis();
  float delta_time = (millis() - last_time) / 1000.f;
  last_time = millis();
  

  if (vol > max(.05, noise_threshold * 1.2f)) {
    intensity = max(intensity, sqrt(sqrt(vol - noise_threshold)));
  }
  
  intensity = max(0, intensity - delta_time);
  
  #ifdef TEST_LED_CIRCLE
  intensity = 1.0;
  #endif
  
  bool off_last_two_frames = (prev_intensity == 0.0f) && (intensity == 0.0f);
  prev_intensity = intensity;
  
  if (off_last_two_frames) {
    LowPowerSleep();
  } else {
    DoVis(intensity * INTENSITY_FACTOR);
    strip.show();
    delay(5);
  }
}
