#include "OctoWS2811.h"
#include "Vis.h"

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:

  pinMode(1, OUTPUT);
  digitalWrite(1, HIGH);
  for (int i=0; i<180; i++) {
    int hue = i * 2;
    int saturation = 100;
    int lightness = 50;
    // pre-compute the 180 rainbow colors
    rainbowColors[i] = makeColor(hue, saturation, lightness);
  }
  digitalWrite(1, LOW);
  leds.begin();
}

void loop() {
  // put your main code here, to run repeatedly:

  int audioRead[] = {
    analogRead(23),
    analogRead(22),
    analogRead(19),
    analogRead(18)
  };
  const int N = sizeof(audioRead) / sizeof(*audioRead);

  for (int i = 0; i < N; ++i) {
    audioRead[i] = map(audioRead[i], 0, 333, 0, 1024);
    audioRead[i] = min(1024, audioRead[i]);
    audioRead[i] = max(0, audioRead[i]);
  }

  for (int i = 0; i < N; ++i) {
    Serial.print(audioRead[i]);
    Serial.print("\t");
  }
  Serial.println();
  rainbow(10);
}

