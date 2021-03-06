// Copyleft (c) 2012, Zach Vorhies
// Public domain, no rights reserved.

#ifndef LEDROPETCL_H_
#define LEDROPETCL_H_

#include <Arduino.h>

// LedRopeTCL is a C++ wrapper around the Total Control Lighting LED rope
// device driver (TCL.h). This wrapper includes automatic setup of the LED
// rope and allows the user to use a graphics-state like interface for
// talking to the rope. A copy of the rope led state is held in this class
// which makes blending operations easier. After all changes by the user
// are applied to the rope, the hardware is updated via an explicit Draw()
// command.
//
// Whole-rope blink Example:
//  #include <SPI.h>
//  #include <TCL.h>          // From CoolNeon (https://bitbucket.org/devries/arduino-tcl)
//  #include "LedRopeTCL.h"
//  LedRopeTCL led_rope(100); // 100 led-strand.
//
//  void setup() {}  // No setup necessary for Led rope.
//  void loop() {
//    led_rope.FillColor(LedRopeTCL::Color3i::Black());
//    led_rope.Draw();
//    delay(1000);
//    led_rope.FillColor(LedRopeTCL::Color3i::White());
//    led_rope.Draw();
//    delay(1000);
//  }
class LedRopeTCL {
 public:
  LedRopeTCL(int n_leds);
  ~LedRopeTCL();

  struct Color3i;

  void Draw();
    
  // same api as Adafruit's Neopixel library
  inline show() { Draw(); }
  void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
  void setPixelColor(uint16_t n, uint32_t c);
    
    
  void Set(int i, const Color3i& c);
  void Set(int i, int length, const Color3i& color) {
    for (int j = 0; j < length; ++j) {
      Set(i + j, color);
    }
  }

  void FillColor(const Color3i& color);

  void ApplyBlendSubtract(const Color3i& color);
  void ApplyBlendAdd(const Color3i& color);
  void ApplyBlendMultiply(const Color3i& color);

  Color3i* GetIterator(int i);

  // Length in pixels.
  int length() const { return n_color_array_; }

  struct Color3i {
    static Color3i Black() { return Color3i(0x0, 0x0, 0x0); }
    static Color3i White() { return Color3i(0xff, 0xff, 0xff); }
    static Color3i Red() { return Color3i(0xff, 0x00, 0x00); }
    static Color3i Orange() { return Color3i(0xff, 0xff / 2,00); }
    static Color3i Yellow() { return Color3i(0xff, 0xff,00); }
    static Color3i Green() { return Color3i(0x00, 0xff, 0x00); }
    static Color3i Cyan() { return Color3i(0x00, 0xff, 0xff); }
    static Color3i Blue() { return Color3i(0x00, 0x00, 0xff); }
    Color3i(byte r, byte g, byte b) { Set(r,g,b); }
    Color3i() { Set(0xff, 0xff, 0xff); }

    void Set(byte r, byte g, byte b) { r_ = r; g_ = g; b_ = b; }
    void Set(const Color3i& c) { Set(c.r_, c.g_, c.b_); }
    void Fill(byte val) { Set(val, val, val); }

    void Mul(const Color3i& other_color);
    void Mul(float scale);  // Input range is 0.0 -> 1.0
    void Mul(byte val) {
	  Mul(Color3i(val, val, val));
	}
    void Sub(const Color3i& color);
    void Add(const Color3i& color);
    const byte Get(int rgb_index) const;
    void Set(int rgb_index, byte val);
    void Fill(byte val) { Set(val, val, val); }

    void Interpolate(const Color3i& other_color, float t);

    byte* At(int rgb_index);
    const byte* At(int rgb_index) const;
    uint32_t Hex() const {
        return uint32_t(r_) << 16 | uint32_t(g_) << 8 | uint32_t(b_);
    }

    byte r_, g_, b_;
  };

 void set_draw_offset(int val) {
   draw_offset_ = constrain(val, 0, n_color_array_);
 }

 private:
  // Needed because % operations will not provide wrappin services. For example:
  // -1 % 10 == -1 on arduino.
  static int Wrap(int val, int max_val);
  Color3i* color_array_;
  int n_color_array_;
  int draw_offset_;
};

// Full declaration of Color3i.


#endif  // LEDROPETCL_H_
