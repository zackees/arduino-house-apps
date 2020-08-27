// This sketch tests the ability of the Arduino to do noise canceling.
// Hardware: 2 eletret mics from sparkfun that produce an analog voltage
// reading.


#include "FastAnalogRead.h"

const int MIC_PIN_0 = A0;
const int MIC_PIN_1 = A1;
#define N_SIZE 16

byte mic_sample_0[N_SIZE];
byte mic_sample_1[N_SIZE];


int CalcVolume(byte* samples, int n_size) {
  if (n_size < 1)
    return 0;
  
  int max_val = samples[0];
  int min_val = samples[0];
  
  for (int i = 1; i < n_size; ++i) {
    int val = samples[i];
    max_val = max(max_val, val);
    min_val = min(min_val, val);
  }
  int vol = max_val - min_val;
  vol = max(0, vol - 64);
  return vol;
}


void setup() {
  Serial.begin(9600);
  EnableFastAnalogRead();  // Sample rate above voice activation.
}

void loop() {
  for (int i = 0; i < N_SIZE; ++i) {
    mic_sample_0[i] = analogRead(MIC_PIN_0) >> 2;
    mic_sample_1[i] = analogRead(MIC_PIN_1) >> 2;
  }
  
  int volume_0 = CalcVolume(mic_sample_0, N_SIZE);
  int volume_1 = CalcVolume(mic_sample_1, N_SIZE);
  if (false) {
    Serial.println(volume_0 << 2);
    Serial.println(volume_1 << 2);
  } else {
    Serial.println(abs(volume_0 - volume_1));
  }
  delay(16);
}
