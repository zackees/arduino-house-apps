//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100 
#define COOLING  30


static const int kSparkCount = 5;
static const int kNumHeat = NUM_LEDS + kSparkCount;
static byte heat[kNumHeat];
static byte heat2[kNumHeat];


// heat is a heat array.
void FirePropogation(byte* heat, int n, int sparking) {
    static const int kSparkCount = 5;

    // Step 1.  Cool down every cell a little
    for( int i = 0; i < n; i++) {
      int cooling_factor = ((COOLING * 10) / n) + 2;
      
      if (heat[i] > 140 || sparking == 0) {
        // max() prevents wrap arounds.
        cooling_factor = max(cooling_factor, 2*cooling_factor);
      } else if (heat[i] < 10) {
        cooling_factor /= 2;
      }      
      heat[i] = qsub8( heat[i],  random8(0, cooling_factor));
    }
  
    // Step 2.  Heat from each cell drifts 'out' and diffuses a little
    for( int k= n - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if(random8() < sparking) {
      int y = random16(kSparkCount/ 2);
      heat[y] = qadd8( heat[y], random8(0,255) );
    }


}

CRGB MyHeatToColor(int heat_color) {
  if (heat_color >= 100)
    heat_color = 100;
  return HeatColor(heat_color);
}

void HeatToLedRope(const byte* heat, int n, LedRopeTCL* led_rope) {
    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < n; j++) {
      CRGB c = MyHeatToColor(heat[j]);
      led_rope->Set(j, Color3i(c[0], c[1], c[2]));
    }
}

byte ByteLerp(int t, int a, int b) {
  int factor_a = ((255-t)*a) >> 8;
  int factor_b = (t*b) >> 8;
  
  int val = factor_a + factor_b;
  return (byte)val;
}

void HeatTransitionToLedRope(const byte* heat1, const byte* heat2, int n,
                             byte lerp, LedRopeTCL* led_rope) {
    for( int j = 0; j < n; j++) {
      byte heat_val = ByteLerp(lerp, heat1[j], heat2[j]);
      CRGB c = MyHeatToColor(heat_val);
      led_rope->Set(j, Color3i(c[0], c[1], c[2]));
    }  
}



void Fire2012(bool fire_enabled, LedRopeTCL* led_rope)
{
    for (int i = 0; i < sizeof(heat); ++i) {
      heat[i] = heat2[i]; 
    }
    FirePropogation(heat2, kNumHeat, fire_enabled ? SPARKING : 0);
    
    unsigned long start_ms = millis();
    
    while (true) {
      const unsigned long frame_time = millis() - start_ms;
      const unsigned long frame_period = 1000 / FRAMES_PER_SECOND;
      
      if (frame_time > frame_period) {
        // Final draw.
        HeatTransitionToLedRope(heat + kSparkCount,
                                heat2 + kSparkCount,
                                kNumHeat - kSparkCount,
                                255, led_rope);
        led_rope->Draw();
        break;
      }
      
      float t = float(frame_time) / float(frame_period);
      byte lerp_value = byte(255.f * t);
      HeatTransitionToLedRope(heat + kSparkCount,
                              heat2 + kSparkCount,
                              kNumHeat - kSparkCount,
                              lerp_value,
                              led_rope);
      led_rope->Draw();
    }
}
    
