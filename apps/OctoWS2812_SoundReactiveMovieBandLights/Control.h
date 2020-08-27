#include "Arduino.h"

enum VisMode {
  RAINBOW,
  RAINBOW_VOL,
  WHITE,
  RED,
  GREEN,
  BLUE,
  MOVIE,
  NOISE_RED_BLUE,
  NOISE_GREEN_BLUE,
  NOISE_RAINBOW,
};

void Control_SetVis(VisMode v);
void Control_Draw(AudioData data);
void Control_Print();

////////////////////////////////////////////////////////////////////////////////////
/// 
/// Control Impl
/// 
////////////////////////////////////////////////////////////////////////////////////

VisMode s_vis_mode = RAINBOW;
VisMode s_prev_vis_mode = RAINBOW;

bool HasVolumeFX(VisMode m) { return m == RAINBOW_VOL; }

VisFunction SelectVis(VisMode m) {
  switch (m) {
    case RAINBOW_VOL:
    case RAINBOW:          { return FillWithRainbow;        }
    case WHITE:            { return FillWithWhite;          }
    case RED:              { return FillWithRed;            }
    case GREEN:            { return FillWithGreen;          }
    case BLUE:             { return FillWithBlue;           }
    case MOVIE:            { return FillWithMovie;          }
    case NOISE_RED_BLUE:   { return FillWithNoiseRedBlue;   }
    case NOISE_GREEN_BLUE: { return FillWithNoiseGreenBlue; }
    case NOISE_RAINBOW:    { return FillWithNoiseRainbow;   }
  }

  SP(__FUNCTION__); SP(" error could not determine visualizer from value: "); SPLN(m);
  return FillWithRainbow;
}

void Control_SetVis(VisMode v) {
  s_prev_vis_mode = s_vis_mode;
  s_vis_mode = v;
}

void Control_Draw(AudioData data) {
  Gfx_Begin();


  unsigned long time_now = millis();
  for (int i = 0; i < AudioData::N; ++i) {
    Gfx_Strip strip;
    VisFunction vis_function = SelectVis(s_vis_mode);
    vis_function(time_now, i, &strip);
    if (HasVolumeFX(s_vis_mode)) {
      ApplyVolumeFX(data.d[i], i, &strip);
    }

    Gfx_SetStrip(i, strip);
  }
  
  Gfx_End();
}

void Control_Print() {
  
}

