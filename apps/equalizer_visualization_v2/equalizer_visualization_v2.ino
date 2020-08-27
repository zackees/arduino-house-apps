
#include <SPI.h>
#include <TCL.h>
#include <LedRopeTCL.h>
#include <SpectrumAnalyzer.h>
#include <Coroutine.h>

#include <NewDeleteExt.h>
#include <Vector.h>
#include <CircularArray.h>

#define SERIAL_BAUD_RATE 57600

static const int kNumLedRopeLights = 150;
//static const int kNumLedRopeLights = 200;
//static const int kNumLedRopeLights = 100;

typedef Color3i ColorPalette[7];
SpectrumAnalyzer spectrum_analyser_;

class IVisualizer {
 public:
  virtual ~IVisualizer() {}
  virtual void Update(const ColorPalette& color_palette,
                      const SpectrumAnalyzer::Output& equalizer,
                      FrameBufferBase* led_rope) = 0;

};

class Visualizer : public IVisualizer{
 public:
  virtual void Update(const ColorPalette& color_palette,
                      const SpectrumAnalyzer::Output& equalizer,
                      FrameBufferBase* led_rope) {

    const float sum_all_equalizer_values = 7 * 255;
    float equalizer_values[7];
    
    for (int i = 0; i < 7; ++i) {
      equalizer_values[i] = float(equalizer.spectrum_array[i]) /
                            sum_all_equalizer_values;
      if (equalizer_values[i] < 0.0f) {
        equalizer_values[i] = 0.0f;
      }
    }
    
    int mid_point = led_rope->length() / 2;
    int forward_pixel_iterator = mid_point;
    int backward_pixel_iterator = mid_point;
    for (int i = 0; i < 7; i++) {
      int num_pixels = int(equalizer_values[i] * led_rope->length() / 2.0f);
      const Color3i& c = color_palette[i];
     
      led_rope->Set(forward_pixel_iterator, num_pixels, c);
      led_rope->Set(backward_pixel_iterator - num_pixels, num_pixels, c);
      forward_pixel_iterator += num_pixels;
      backward_pixel_iterator -= num_pixels;
    }
  }
};


class Visualizer2 : public IVisualizer {
 public:

  virtual void Update(const ColorPalette& color_palatte,
                      const SpectrumAnalyzer::Output& equalizer,
                      FrameBufferBase* led_rope) {
    const int n_bands = equalizer.spectrum_array_size;
    
    float equalizer_values[SpectrumAnalyzer::Output::spectrum_array_size];
    
    for (int i = 0; i < n_bands; ++i) {
      int sound_band_value = equalizer.spectrum_array[i];
      equalizer_values[i] = float(equalizer.spectrum_array[i]) /
                            255.f;
    }
    
    const int strand_length = led_rope->length() / n_bands;
    
    for (int i = 0; i < n_bands; ++i) {
      Color3i* begin_pixel = led_rope->GetIterator(strand_length * i);
      Color3i* end_pixel = led_rope->GetIterator(strand_length * (i + 1));
      
      ApplySpectrum(color_palatte[i], begin_pixel, end_pixel, equalizer_values[i]);
    }
  }
  
 private:
  // Input:
  //   color: Color to paint the strand.
  //   begin/end: Slice of the Color Array.
  //   intensity: A value from [0-1] where 0 means black is painted in all pixels
  //              while 1 means input color is painted in all colors.
  void ApplySpectrum(const Color3i& color,
                     Color3i* begin, Color3i* end,
                     float intensity) {
                       
    const int n = end - begin;
    Color3i* mid = begin + (n / 2);
    
    const int count = static_cast<int>(intensity * static_cast<float>(n / 2));
    for (int i = 0; i < count; ++i) {
      Color3i* forward = mid + i;
      Color3i* backward = mid - i;
      forward->Set(color);
      backward->Set(color);
    }
  }
};


class Visualizer3 : public IVisualizer {
 public:
  virtual void Update(const ColorPalette& color_palette,
                      const SpectrumAnalyzer::Output& equalizer,
                      FrameBufferBase* led_rope) {
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
    
    
    int mid_point = led_rope->length() / 2;
    int forward_pixel_iterator = mid_point;
    int backward_pixel_iterator = mid_point;
    
    const float led_strip_size = led_rope->length() / 7;
    
    for (int i = 0; i < 3; i++) {
      int num_pixels = int(equalizer_values[i] * led_strip_size) / 2;
      const Color3i& c = color_palette[i];
     
      led_rope->Set(forward_pixel_iterator, num_pixels, c);
      led_rope->Set(backward_pixel_iterator - num_pixels, num_pixels, c);
      forward_pixel_iterator += num_pixels;
      backward_pixel_iterator -= num_pixels;
    }
    
    forward_pixel_iterator = 0;
    backward_pixel_iterator = 0;
    
    for (int i = 3; i < 7; i++) {
      int num_pixels = int(equalizer_values[i] * led_strip_size) / 2;
      const Color3i& c = color_palette[i];

      led_rope->Set(forward_pixel_iterator, num_pixels, c);
      led_rope->Set(backward_pixel_iterator - num_pixels, num_pixels, c);
      forward_pixel_iterator += num_pixels;
      backward_pixel_iterator -= num_pixels;
    }
  }
};

LedRopeTCL led_rope(kNumLedRopeLights);
Visualizer vis1;
Visualizer2 vis2;
Visualizer3 vis3;

void setup(void) {
  Serial.begin(SERIAL_BAUD_RATE);
  led_rope.FillColor(Color3i::Black());
  Serial.println("STARTING UP");
}

SpectrumAnalyzer::Output left, right;
void loop(void) {
  delay(8);
  

  spectrum_analyser_.ProcessAudio(&left, &right);
  
  left.DebugPrint(Serial);
  left.DebugPrint(Serial);
 

  //  sound_equalizer.PrintBands(sound_equalizer_output); 
  // Fades the rope by 50% on each color.
  led_rope.ApplyBlendMultiply(Color3i(128, 128, 128));

  static Color3i color_palette[] = {
    Color3i::Red(),
    Color3i::Orange(),
    Color3i::Yellow(),
    Color3i::Green(),
    Color3i::Cyan(),
    Color3i::Blue(),
    Color3i(255, 0, 255), // purple
  };


  static int counter = 0;
  counter = (counter + 1) % 1500;

  if (counter < 500) {
    //spectrum_analyser_.set_gain(1.0f);
    //vis3.Update(color_palette, left, &led_rope);
  } else if (counter < 1000) {
    spectrum_analyser_.set_gain(1.0f);
    vis2.Update(color_palette, left, &led_rope); 
  } else {
    spectrum_analyser_.set_gain(2.0f);
    vis1.Update(color_palette, left, &led_rope); 
  }
  
  static int draw_offset = 0;
  if (counter % 10 == 0) {
    draw_offset = (draw_offset + 1) % led_rope.length();
    led_rope.set_draw_offset(draw_offset);
  }
 #if 0
  led_rope.Draw();
  #endif
}
