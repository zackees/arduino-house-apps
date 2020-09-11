

#include "fx.h"
#include "darkness.h"

#include "defs.h"

///////////////////////////////////////////////////
// Describes a brightness for each LED in the array, according to time_now
// progressing.
class FxEaseIn {
 public:
  FxEaseIn() { Init(); }
  void Start(uint32_t now) {
    // go from 1->0 in 10 seconds (update: values might be updated from comment.)
    interp_anim_.SetAll(1.0f, -0.5f, 5.0f, now);
  }
  
  void Update(uint32_t now, int length) {
    float t = interp_anim_.Value(now);
    mid_point_ = length / 2;
    dist_full_color_ = t * mid_point_;
    dist_full_black_ = (t + .5) * mid_point_;
  }
  
  float Brightness(int i) const {
    int dist = abs(mid_point_ - i);
    float b = mapf(dist, dist_full_color_, dist_full_black_, 0.0f, 1.0f);
    b = clampf(b, 0.0f, 1.0f);
    return b;
  }
 private:
  void Init() {
    dist_full_black_ = 0;
    dist_full_color_ = 0;
  }
  InterpAnimator interp_anim_;
  float dist_full_color_;
  float dist_full_black_;
  int mid_point_;
};


enum State {
  kAllBlack = 0,
  kEaseIn = 1,
  kAllOn = 2,
  kEaseOut = 3,
};

const char* ToString(State s) {
  switch (s) {
      case kAllBlack: return "kAllBlack";
      case kEaseIn: return "kEaseIn";
      case kAllOn: return "kAllOn";
      case kEaseOut: return "kEaseOut";
      default: return "ERROR - Unknown";
  }
}

class FxControl {
 public:
  State mCurrState = kAllBlack;
  uint32_t mLastTriggeredTime = 0;
  uint32_t mCurrStateStartTime = 0;
  FxEaseIn mFxEaseIn;

  FxControl() : mCurrState(kAllBlack), mLastTriggeredTime(0), mCurrStateStartTime(0) {}
  
  void Update(uint32_t now, bool triggered) {
  	if (triggered) {
  	  mLastTriggeredTime = now;
  	}
  	uint32_t trigger_time = now - mLastTriggeredTime;
  	switch (mCurrState) {
  	  case kAllBlack: {
  	  	if (mLastTriggeredTime > 0 && trigger_time < 1000 * 10) {
  	  	  mCurrState = kEaseIn;
  	  	  mCurrStateStartTime = now;
          mFxEaseIn.Start(now);
  	  	}
  		break;		
  	  }
  	  case kEaseIn: {
        const uint32_t total_state_time = now - mCurrStateStartTime;
  	  	if (total_state_time > FX_EASE_IN_TIME) {
  	  	  mCurrState = kAllOn;
  	  	  mCurrStateStartTime = now;
  	  	}
  	  	break;
  	  }
  	  case kAllOn: {
  	  	if (trigger_time > FX_ALL_ON_TIME) {
  	  	  mCurrState = kEaseOut;
  	  	  mCurrStateStartTime = now;
  	  	}
  	  	break;
  	  }
  	  case kEaseOut: {
  	  	if ((now - mCurrStateStartTime) > FX_EASE_OUT_TIME) {
  	  	  mCurrState = kAllBlack;
  	  	  mCurrStateStartTime = now;
  	  	}
  	  	break;
  	  }
  	}
  }

  void Apply(uint32_t now, CRGB* dest, int num_leds) {
    switch (mCurrState) {
      case kAllBlack: {
        memset(dest, 0, sizeof(*dest) * num_leds);
        break;
      }
      case kEaseIn: {
        mFxEaseIn.Update(now, num_leds);
        for (int i = 0; i < num_leds; ++i) {
          float b = mFxEaseIn.Brightness(i);
          if (b <= 0.0f) {
            display_leds[i] = CRGB::Black;
          }
          if (b < 1.0f) {
            display_leds[i] = CRGB(display_leds[i].r * b, display_leds[i].g * b, display_leds[i].b * b);
          }
        }
        break;
      }
      case kAllOn: {
        break;
      }
      case kEaseOut: {
        uint32_t time_running = now - mCurrStateStartTime;
        float b = mapf(time_running, 0, FX_EASE_OUT_TIME, 1, 0);
        for (int i = 0; i < NUM_LEDS; ++i) {
          dest[i] = CRGB(dest[i].r * b, dest[i].g * b, dest[i].b * b);
        }
        break;
      }
    }
  }
};


static FxControl fx_control;

void fx_update(uint32_t time_now, bool triggered, CRGB* dest, int num_leds) {
  fx_control.Update(time_now, triggered);
  fx_control.Apply(time_now, dest, num_leds);
}

