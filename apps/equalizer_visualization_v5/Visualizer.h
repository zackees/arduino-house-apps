
#ifndef _VISUALIZER_H_
#define _VISUALIZER_H_

#include "Arduino.h"



typedef Color3i ColorPalette[7];

class IVisualizer {
 public:
  virtual ~IVisualizer() {}
  virtual void Update(const ColorPalette& color_palette,
                      const SpectrumAnalyzer::Output& equalizer,
                      CRGB* led_rope) = 0;

};

class Visualizer : public IVisualizer{
 public:
  virtual void Update(const ColorPalette& color_palette,
                      const SpectrumAnalyzer::Output& equalizer,
                      CRGB* led_rope) {

    const float sum_all_equalizer_values = 7 * 255;
    float equalizer_values[7];
    
    for (int i = 0; i < 7; ++i) {
      equalizer_values[i] = float(equalizer.spectrum_array[i]) /
                            sum_all_equalizer_values;
      if (equalizer_values[i] < 0.0f) {
        equalizer_values[i] = 0.0f;
      }
    }
    
    int mid_point = NUM_LEDS / 2;
    int forward_pixel_iterator = mid_point;
    int backward_pixel_iterator = mid_point;
    for (int i = 0; i < 7; i++) {
      int num_pixels = int(equalizer_values[i] * NUM_LEDS / 2.0f);
      const Color3i& c = color_palette[i];
     
      // led_rope->Set(forward_pixel_iterator, NUM_LEDS, c);
      led_rope[forward_pixel_iterator] = c;
      
      //led_rope->Set(backward_pixel_iterator - num_pixels, num_pixels, c);
      led_rope[backward_pixel_iterator] = c;
      
      forward_pixel_iterator += num_pixels;
      backward_pixel_iterator -= num_pixels;
    }
  }
};


class Visualizer2 : public IVisualizer {
 public:

  virtual void Update(const ColorPalette& color_palatte,
                      const SpectrumAnalyzer::Output& equalizer,
                      CRGB* led_rope) {
    const int n_bands = equalizer.spectrum_array_size;
    
    float equalizer_values[SpectrumAnalyzer::Output::spectrum_array_size];
    
    for (int i = 0; i < n_bands; ++i) {
      int sound_band_value = equalizer.spectrum_array[i];
      equalizer_values[i] = float(equalizer.spectrum_array[i]) /
                            255.f;
    }
    
    const int strand_length = NUM_LEDS / n_bands;
    
    for (int i = 0; i < n_bands; ++i) {
      ApplySpectrum(color_palatte[i],
                    equalizer_values[i],
                    strand_length * i,
                    strand_length * (i + 1),
                    led_rope);
    }
  }
  
 private:
  // Input:
  //   color: Color to paint the strand.
  //   begin/end: Slice of the Color Array.
  //   intensity: A value from [0-1] where 0 means black is painted in all pixels
  //              while 1 means input color is painted in all colors.
  void ApplySpectrum(const Color3i& color,
                     float intensity,
                     int start_idx, int end_idx,
                     CRGB* pixel_array) {
                       
                       
                       
    const int n = end_idx - start_idx;
    int mid_idx = start_idx + (n / 2);
    
    const int count = static_cast<int>(intensity * static_cast<float>(n / 2));
    for (int i = 0; i < count; ++i) {
      //pixel_array->Set(mid_idx + i, color);
      pixel_array[mid_idx + i] = color;
      //pixel_array->Set(mid_idx - i, color);
      pixel_array[mid_idx - i] = color;
    }
  }
};


class Visualizer3 : public IVisualizer {
 public:
  virtual void Update(const ColorPalette& color_palette,
                      const SpectrumAnalyzer::Output& equalizer,
                      CRGB* led_rope) {
    const int n_bands = equalizer.spectrum_array_size;
    
    // Normalized equalizer values.
    float equalizer_values[SpectrumAnalyzer::Output::spectrum_array_size];
    for (int i = 0; i < n_bands; ++i) {
      uint8_t val = equalizer.spectrum_array[i];
      equalizer_values[i] = float(val) / 255.f;
      if (equalizer_values[i] < 0.0f) {
        equalizer_values[i] = 0.0f;
      }
    }
    
    
    int mid_point = NUM_LEDS / 2;
    int forward_pixel_iterator = mid_point;
    int backward_pixel_iterator = mid_point;
    
    const float led_strip_size = NUM_LEDS / 7;
    
    for (int i = 0; i < 3; i++) {
      int num_pixels = int(equalizer_values[i] * led_strip_size) / 2;
      const Color3i& c = color_palette[i];
     
      //led_rope->Set(forward_pixel_iterator, num_pixels, c);
      Set(led_rope, forward_pixel_iterator, num_pixels, c);
      
      //led_rope->Set(backward_pixel_iterator - num_pixels, num_pixels, c);
      Set(led_rope, backward_pixel_iterator - num_pixels, num_pixels, c);
      forward_pixel_iterator += num_pixels;
      backward_pixel_iterator -= num_pixels;
    }
    
    forward_pixel_iterator = 0;
    backward_pixel_iterator = 0;
    
    for (int i = 3; i < 7; i++) {
      int num_pixels = int(equalizer_values[i] * led_strip_size) / 2;
      const Color3i& c = color_palette[i];

      //led_rope->Set(forward_pixel_iterator, num_pixels, c);
      Set(led_rope, forward_pixel_iterator, num_pixels, c);
      //led_rope->Set(backward_pixel_iterator - num_pixels, num_pixels, c);
      Set(led_rope, backward_pixel_iterator - num_pixels, num_pixels, c);
      forward_pixel_iterator += num_pixels;
      backward_pixel_iterator -= num_pixels;
    }
  }
};


class RotatingVis : public IVisualizer {
 public:
  RotatingVis(IVisualizer** visualizers, int n)
    : array_(visualizers), n_array_(n), counter_(0) {
    
  }
  
  virtual void Update(const ColorPalette& color_palette,
                      const SpectrumAnalyzer::Output& equalizer,
                      CRGB* led_rope) {
    ++counter_;
    if (0 == (counter_ % 512)) {
      curr_idx_ = (curr_idx_ + 1) % n_array_;
    }
    IVisualizer* vis = array_[curr_idx_];
    //led_rope->SetRotationalOffset(counter_);
    vis->Update(color_palette, equalizer, led_rope);
    //led_rope->SetRotationalOffset(0);
  }
  
 private:
  IVisualizer** array_;
  int n_array_;
  unsigned int counter_;
  int curr_idx_;
};

#endif  // 

