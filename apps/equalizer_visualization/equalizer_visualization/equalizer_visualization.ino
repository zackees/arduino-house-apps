
#include <SPI.h>
#include <TCL.h>
#include <LedRopeTCL.h>
#include <SpectrumAnalyzer.h>

// #define DEBUG

static const int kNumLedRopeLights = 150;

LedRopeTCL led_rope(kNumLedRopeLights);
SpectrumAnalyzer sound_equalizer;

void setup(void) {
  Serial.begin(9600);
  led_rope.FillColor(Color3i::Black());
  sound_equalizer.set_gain(2.f);
}

void loop(void) {
  delay(8);
  
 
  SpectrumAnalyzer::Output left_channel, right_channel;
  sound_equalizer.ProcessAudio(&left_channel, &right_channel);
  
  #ifdef DEBUG
  sound_equalizer.PrintBands(right_channel);
  #endif  // DEBUG
  
  // Fades the rope by 50% on each color.
  led_rope.ApplyBlendMultiply(Color3i(128, 128, 128));
  
//  sound_equalizer.PrintBands(sound_equalizer_output);
  
  Color3i color_band[] = {
    Color3i::Red(),
    Color3i::Orange(),
    Color3i::Yellow(),
    Color3i::Green(),
    Color3i::Cyan(),
    Color3i(0x00, 0xff / 2, 0xff),
    Color3i::Blue()
  };
  
  const float sum_all_equalizer_values = 7 * 255;
  float values_left[7], values_right[7];
  
  for (int i = 0; i < 7; ++i) {
    int sound_band_value = left_channel.spectrum_array[i];
    values_left[i] = float(left_channel.spectrum_array[i]) /
                           sum_all_equalizer_values;
  }
  
  for (int i = 0; i < 7; ++i) {
    int sound_band_value = left_channel.spectrum_array[i];
    values_right[i] = float(right_channel.spectrum_array[i]) /
                            sum_all_equalizer_values;
  }
  
  int mid_point = kNumLedRopeLights / 2;
  int forward_pixel_iterator = mid_point;
  int backward_pixel_iterator = mid_point;
  for (int i = 0; i < 7; i++) {
    int num_pixels = int(values_left[i] * kNumLedRopeLights / 2.0f);
    const Color3i& c = color_band[i];
   
    led_rope.Set(forward_pixel_iterator, num_pixels, c);
    led_rope.Set(backward_pixel_iterator - num_pixels - 1, num_pixels, c);
    forward_pixel_iterator += num_pixels;
    backward_pixel_iterator -= num_pixels;
  }
  led_rope.Draw();
}
