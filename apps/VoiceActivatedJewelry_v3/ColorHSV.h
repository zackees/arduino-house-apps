#include <Arduino.h>

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
  Color3i(const Color3i& other) { Set(other); }

  void Set(byte r, byte g, byte b) { r_ = r; g_ = g; b_ = b; }
  void Set(const Color3i& c) { Set(c.r_, c.g_, c.b_); }
  void Mul(const Color3i& other_color);
  void Mulf(float scale);  // Input range is 0.0 -> 1.0
  void Mul(byte val) {
    Mul(Color3i(val, val, val));
  }
  void Sub(const Color3i& color);
  void Add(const Color3i& color);
  const byte Get(int rgb_index) const;
  void Set(int rgb_index, byte val);
  void Fill(byte val) { Set(val, val, val); }
  byte MaxRGB() const {
    byte max_r_g = r_ > g_ ? r_ : g_;
    return max_r_g > b_ ? max_r_g : b_;
  }

  inline void Print(HardwareSerial* serial) const {
	serial->print("RGB:\t");
    serial->print(r_); serial->print(",\t");
    serial->print(g_); serial->print(",\t");
    serial->println(b_);
  }

  void Interpolate(const Color3i& other_color, float t);

  byte* At(int rgb_index);
  const byte* At(int rgb_index) const;

  byte r_, g_, b_;
};

struct ColorHSV {
  ColorHSV() : h_(0), s_(0), v_(0) {}
  ColorHSV(float h, float s, float v) {
    Set(h,s,v);
  }
  explicit ColorHSV(const Color3i& color) {
    FromRGB(color);
  }
  ColorHSV(const ColorHSV& other) {
    Set(other);
  }
  void Set(const ColorHSV& other) {
	Set(other.h_, other.s_, other.v_);
  }
  void Set(float h, float s, float v) {
    h_ = h;
    s_ = s;
    v_ = v;
  }
  inline void Print(HardwareSerial* serial) {
	serial->print("HSV:\t");
    serial->print(h_); serial->print(",\t");
    serial->print(s_); serial->print(",\t");
    serial->println(v_);
  }

  bool operator==(const ColorHSV& other) const {
    return h_ == other.h_ && s_ == other.s_ && v_ == other.v_;
  }
  bool operator!=(const ColorHSV& other) const {
    return !(*this == other);
  }
  void FromRGB(const Color3i& rgb) {
    typedef double FloatT;
    FloatT r = (FloatT) rgb.r_/255.f;
    FloatT g = (FloatT) rgb.g_/255.f;
    FloatT b = (FloatT) rgb.b_/255.f;
    FloatT max_rgb = max(r, max(g, b));
    FloatT min_rgb = min(r, min(g, b));
    v_ = max_rgb;

    FloatT d = max_rgb - min_rgb;
    s_ = max_rgb == 0 ? 0 : d / max_rgb;

    if (max_rgb == min_rgb) {
      h_ = 0; // achromatic
    } else {
      if (max_rgb == r) {
        h_ = (g - b) / d + (g < b ? 6 : 0);
      } else if (max_rgb == g) {
        h_ = (b - r) / d + 2;
      } else if (max_rgb == b) {
        h_ = (r - g) / d + 4;
      }
      h_ /= 6;
    }
  }

  Color3i ToRGB() const {
    typedef double FloatT;
    FloatT r, g, b;

    int i = int(h_ * 6);
    FloatT f = h_ * 6.0 - static_cast<FloatT>(i);
    FloatT p = v_ * (1.0 - s_);
    FloatT q = v_ * (1.0 - f * s_);
    FloatT t = v_ * (1.0 - (1.0 - f) * s_);

    switch(i % 6){
      case 0: r = v_, g = t, b = p; break;
      case 1: r = q, g = v_, b = p; break;
      case 2: r = p, g = v_, b = t; break;
      case 3: r = p, g = q, b = v_; break;
      case 4: r = t, g = p, b = v_; break;
      case 5: r = v_, g = p, b = q; break;
    }

    return Color3i(round(r * 255), round(g * 255), round(b * 255));
  }

  float h_, s_, v_;
};

