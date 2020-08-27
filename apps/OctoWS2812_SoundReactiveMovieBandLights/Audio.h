
#include "Arduino.h"

#define AUDIO_PIN0 23
#define AUDIO_PIN1 22
#define AUDIO_PIN2 19
#define AUDIO_PIN3 18

#define N_AUDIO_CHANNELS 4

#define N_AUDIO_SAMPLES 50

const int AUDIO_PIN_MAP[N_AUDIO_CHANNELS] = { AUDIO_PIN0, AUDIO_PIN1, AUDIO_PIN2, AUDIO_PIN3 };


void Audio_Setup() {
  for (int i = 0; i < ARRAYSIZE(AUDIO_PIN_MAP); ++i) {
    int pin = AUDIO_PIN_MAP[i];
    pinMode(pin, INPUT);
  }
}


struct AudioData {
  static const int N = 4;
  uint8_t d[N];
};




AudioData ReadAudio() {
  PERF_TIMER;
  AudioData data;

  int max_val[N_AUDIO_CHANNELS];
  memset(max_val, 0, sizeof(max_val));


  // Read all audio pins as near concurrently as possible.
  for (int i = 0; i < N_AUDIO_SAMPLES; ++i) {
    for (int ii = 0; ii < N_AUDIO_CHANNELS; ++ii) {
      int val = analogRead(AUDIO_PIN_MAP[ii]);
      int& mval = max_val[ii];
      mval = max(mval, val);
      if (mval < 6) { mval = 0; }
    }
  }

  for (int i = 0; i < N_AUDIO_CHANNELS; ++i) {
    // apply a squared function to make the high points more dramatic.
    data.d[i] = static_cast<uint8_t>(max_val[i] * max_val[i] / 255);
  }

  return data;
}

void PrintAudio(const AudioData& data) {
  for (int i = 0; i < data.N; ++i) {
    SP(data.d[i]); SP("\t");
  }
  Serial.println();
}



