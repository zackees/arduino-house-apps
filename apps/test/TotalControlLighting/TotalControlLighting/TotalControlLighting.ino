
#include <SPI.h>
#include <TCL.h>
#include <LedRopeTCL.h>

LedRopeTCL led_rope(50);

const int kDelay = 50;

static void TestHSVConverstion() {
  ColorHSV black_hsv(Color3i::Black());
  ColorHSV red_hsv(Color3i::Red());
  ColorHSV cyan_hsv(Color3i::Cyan());
  
  //#define P(c) Serial.print("\t"); Serial.print(#c); Serial.print(":\t"); c.Print(&Serial);
  #define P(c) Serial.println("hi!");
  
  P(black_hsv);
  P(red_hsv);
  P(cyan_hsv);
  
  Color3i black = black_hsv.ToRGB();
  Color3i red = red_hsv.ToRGB();
  Color3i cyan = cyan_hsv.ToRGB();
  
  P(black);
  P(red);
  P(cyan);

  #undef P
  
  delay(2000);
}



void setup(void) {
  Serial.begin(9600);
  led_rope.FillColor(Color3i::Black());
  led_rope.Draw();
  delay(750);
  led_rope.FillColor(Color3i::Red());
  led_rope.Draw();
  delay(750);
  led_rope.FillColor(Color3i::Green());
  led_rope.Draw();
  delay(750);
  led_rope.FillColor(Color3i::Blue());
  led_rope.Draw();
  delay(750);
}


void loop(void) {
  // TestHSVConverstion();
  
  setup();
  return;
  
  Serial.println("hi!");

  Color3i colors[] = {
    Color3i::Red(),
    Color3i::Green(),
    Color3i::Blue()
  };
  
  Color3i fade_to_black(192, 192, 192);
  
  for (int c = 0; c < 3; ++c) {
    Color3i color = colors[c];
    for (int i = 0; i < led_rope.length(); ++i) {
      led_rope.ApplyBlendMultiply(fade_to_black);
      led_rope.Set(i, color);
      led_rope.Draw();
      delay(kDelay);
    }
  }
  
  Serial.println("A");
  
  // Finally fade everything to black.
  for (int i = 0; i < 8; ++i) {
    led_rope.ApplyBlendMultiply(fade_to_black);
    led_rope.Draw();
    delay(kDelay);
  }
  
  Serial.println("B");

}
