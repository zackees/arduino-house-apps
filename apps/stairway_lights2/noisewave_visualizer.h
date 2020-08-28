
//#define VIS_DURATION 1000 * 60 * 10  // 10 minutes
#define VIS_DURATION 1000 * 5   // 5 seconds


DurationTimer activeTimer;




void vis_loop() {

  static int global_brightness = 0;


  // test fucnction for the pir motion sensor.

  bool internal_sig = digitalRead(PIN_PIR) == HIGH;
  int external_sig = digitalRead(PIN_EXTERNAL_SIG) == LOW;

  

  if (external_sig || internal_sig) {
    activeTimer.Restart(VIS_DURATION);
  }

  unsigned long time_now = millis();

  bool increase_brightness = false;
  if (activeTimer.Active()) { increase_brightness = true; }

  if (increase_brightness) {
    global_brightness += 4;
  } else {
    global_brightness--;
  }


  if (global_brightness < -255) {
    global_brightness = -255;
  } else if (global_brightness > 0) {
    global_brightness = 0;
  }



  {
    unsigned long start_t = millis();
    unsigned long time_now = start_t;


    NoiseGenerator noiseGeneratorRed (500, 14);
    NoiseGenerator noiseGeneratorBlue (500, 10);
  
    for (int32_t i = 0; i < NUM_LEDS; ++i) {
     
      int r = noiseGeneratorRed.LedValue(i, time_now);
      int b = noiseGeneratorBlue.LedValue(i, time_now + 100000) >> 1;
      int g = 0;
  
      if (global_brightness < 0) {
        r = max(0, r + global_brightness);
        g = max(0, g + global_brightness);
        b = max(0, b + global_brightness);
      }
 
      leds[i].r = r;
      leds[i].g = g;
      leds[i].b = b;
    }


   
    unsigned long delta_t = millis() - start_t;
    Serial.print("strip population took: "); Serial.print(delta_t); Serial.println("ms");
  }

  {
    unsigned long start_t = millis();
    FastLED.show();
    unsigned long delta_t = millis() - start_t;
    Serial.print("strip.show() took: "); Serial.print(delta_t); Serial.println("ms");
  }
}
