
#ifndef _DRUM_VIS_H_
#define _DRUM_VIS_H_

#include <Arduino.h>

#include "hsv2rgb.h"

#include "Visualizer.h"

CHSV rgb2hsv_approximate( const CRGB& rgb);


class BeatCounter {
 public:
  BeatCounter() : prev_volume_(0), curr_volume_(0), counter_(0), prev_time_(0),
                  volume_history_(16) {
  }
  int UpdateAndGet(int max_volume, unsigned long now_ms, bool* is_loud_beat, bool* is_beat) {
    if (max_volume < 75)
      max_volume = 0;
      
    const int prev_counter = counter_;
    const bool was_increasing = (curr_volume_ > prev_volume_);
    prev_volume_ = curr_volume_;
    curr_volume_ = max_volume;
    
    
    const bool in_cooldown = (prev_time_ + cooldown_time_ms) > now_ms;
    const bool curr_is_beat_loud = IsBeatLoud(max_volume);
    
   
    
    if (!in_cooldown) {      
      *is_loud_beat = curr_is_beat_loud;
    } else {
      *is_loud_beat = false;
    }
    
    const bool is_decreasing = curr_volume_ < prev_volume_;
    
    
    if (is_decreasing && was_increasing) {
      // Volume just started falling on this frame.
      
      
      const bool in_cooldown = (prev_time_ + cooldown_time_ms) > now_ms;
      if (!in_cooldown) {
        counter_++;
        prev_time_ = now_ms;
        volume_history_.pop_front();
        volume_history_.push_back(max_volume);
      }
    }
    *is_beat = prev_counter != counter_;
    return counter_;
  }
  
  bool IsBeatLoud(int beat_volume) {
    int max_volume = 0;
    for (int i = 0; i < volume_history_.size(); ++i) {
      max_volume = max(max_volume, volume_history_[i]);
    }
    return max_volume < beat_volume;
  }
  
  static const unsigned int cooldown_time_ms = 30;
  static const unsigned int cooldown_loud_beat_ms = 300;
  int prev_volume_;
  int curr_volume_;
  unsigned long prev_time_;
  int counter_;
  Vector<int> volume_history_;
};

class VolumeDecay {
 public:
  VolumeDecay() : curr_volume_(0), prev_time_(0) {}
  
  float UpdateAndGet(float volume, unsigned long now_ms) {
    if (volume > curr_volume_) {
      curr_volume_ = volume;
    } else {
      float dt = (now_ms - prev_time_) / 1000.0f;
      curr_volume_ = max(0.0, curr_volume_ * exp(-10.0 * dt));
      //curr_volume_ = max(0.0, curr_volume_ - (2.0f * dt));
    }
    prev_time_ = now_ms;
    return curr_volume_;
  }
  
  float curr_volume_;
  float prev_time_;
};


class BeatProcessor {
 public:
  void Update(const SpectrumAnalyzer::Output& equalizer,
              unsigned long time_ms,
              bool* is_beat_loud,
              bool* is_beat,
              int* beat_num,
              float* volume) {
     // Alias
     const uint8_t* array = equalizer.spectrum_array;
     

     
     // Find maximum decibel from all channels.
     uint8_t max_volume = 0;
     for (int i = 0; i < 3; ++i) {
       if (array[i] > max_volume)
         max_volume = array[i];
     }
    
    *beat_num = beat_counter_.UpdateAndGet(int(max_volume), time_ms, is_beat_loud, is_beat);
    *volume = vol_decay_.UpdateAndGet(float(max_volume < 50 ? 0 : max_volume) / 255.f, time_ms);    
  }
  
  BeatCounter beat_counter_;
  VolumeDecay vol_decay_;
};



class DrumVis : public IVisualizer {
 public:
                      

   DrumVis() : idx_(0) {}                      
   virtual void Update(const ColorPalette& color_palette,
                       const SpectrumAnalyzer::Output& equalizer,
                       CRGB* led_rope) {
                         
                         
     bool is_beat = false;
     bool is_beat_loud = false;
     int beat_num = 0;
     float volume = 0;
                         
     beat_processor_.Update(equalizer, millis(),
                            &is_beat_loud,
                            &is_beat,
                            &beat_num,
                            &volume);
    
    
    static ColorHSV s_color_wheel(0, 1, 1);
    
    ColorHSV init_c(0,0,0);
    
    Color3i palette_rgb[] = {
        Color3i(0xff, 0x00, 0x00), // C
        Color3i(0x00, 0x80, 0xff), // C
        Color3i(0xff, 0xff, 0x00), // D
        Color3i(0x80, 0x00, 0xff), // D#
        Color3i(0x00, 0xff, 0x00), // E
        Color3i(0xff, 0x00, 0x80), // F
        Color3i(0x00, 0xff, 0xff), // F#
        Color3i(0xff, 0x80, 0x00), // G
        Color3i(0x00, 0x00, 0xff), // G#
        Color3i(0x80, 0xff, 0x00), // A
        Color3i(0xff, 0x00, 0xff), // A#
        Color3i(0x00, 0xff, 0x80)
    };
    
    ColorHSV palette[] = {
        init_c, //ColorHSV(Color3i(0xff, 0x00, 0x00)), // C
        init_c, //ColorHSV(Color3i(0x00, 0x80, 0xff)), // C
        init_c, //ColorHSV(Color3i(0xff, 0xff, 0x00)), // D
        init_c, //ColorHSV(Color3i(0x80, 0x00, 0xff)), // D#
        init_c, //ColorHSV(Color3i(0x00, 0xff, 0x00)), // E
        init_c, //ColorHSV(Color3i(0xff, 0x00, 0x80)), // F
        init_c, //ColorHSV(Color3i(0x00, 0xff, 0xff)), // F#
        init_c, //ColorHSV(Color3i(0xff, 0x80, 0x00)), // G
        init_c, // ColorHSV(Color3i(0x00, 0x00, 0xff)), // G#
        init_c, //ColorHSV(Color3i(0x80, 0xff, 0x00)), // A
        init_c, //ColorHSV(Color3i(0xff, 0x00, 0xff)), // A#
        init_c, // ColorHSV(Color3i(0x00, 0xff, 0x80))
    };
    
    for (int i = 0; i < ARRAY_SIZE(palette_rgb); ++i) {
      palette[i] = rgb2hsv_approximate(palette_rgb[i]);
    }
    
    
    
    const int n = sizeof(palette) / sizeof(*palette);
    if (is_beat_loud) {
      
      idx_ = (idx_ + 1) % n;
      

      s_color_wheel.h += palette[idx_].h;
      
    }
    ColorHSV c = s_color_wheel;
    c.v = volume;
    //led_rope->SetAll(c.ToRGB());
    SetAll(led_rope, c);
  }
                      
 private:
  int idx_;
  BeatProcessor beat_processor_;
};

#if 0  // TODO: Finish smooth scroll of lights for beats.
class RotationSpeedProcessor {
 public:
  RotationSpeedProcessor() : speed_(0) {}
  void Update(bool is_beat, bool is_beat_loud) {
    speed_ += is_beat * .1;
  }
  
 private:
  float speed_;
};
#endif


class DrumVis2 : public IVisualizer {
 public:
                      

   DrumVis2() : idx_(0) {}                      
   virtual void Update(const ColorPalette& color_palette,
                       const SpectrumAnalyzer::Output& equalizer,
                       CRGB* led_rope) {
                         
     bool is_beat = false;
     bool is_beat_loud = false;
     int beat_num = 0;
     float volume = 0;
                         
     beat_processor_.Update(equalizer, millis(),
                            &is_beat_loud,
                            &is_beat,
                            &beat_num,
                            &volume);

    
    const Color3i palette[] = {
      Color3i::Blue,
      Color3i::Blue,
      Color3i::Blue,
      Color3i::Blue,
      Color3i::Blue,
      Color3i::Blue,
      Color3i::Blue,
    };
    
    //ColorHSV c(Color3i::Blue);
    ColorHSV c = rgb2hsv_approximate(Color3i::Blue);

    c.v = volume;
    Color3i c_rgb = c;
    
    //const int n = led_rope->Length();
    const int n = NUM_LEDS;
    
    //rgb2hsv_approximate
    for (int i = 0; i < n / 3; ++i) {
      led_rope[i] = c_rgb;
    }
    
    for (int i = n / 3; i < n; ++i) {
      //led_rope->Set(i, Color3i::Black());
      led_rope[i] = Color3i::Black;
    }
    
    
    static int counter_ = 0;
    
    counter_ += is_beat * 2 + is_beat_loud * 6;
    //led_rope->SetRotationalOffset(counter_); // disabled, for now.
  }
                      
 private:
  int idx_;
  BeatProcessor beat_processor_;
  
  //Visualizer2 vis_;
};



#endif // _DRUM_VIS_H_
