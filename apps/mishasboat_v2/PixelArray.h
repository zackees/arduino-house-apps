
#ifndef _ABSTRACT_PIXEL_ARRAY_
#define _ABSTRACT_PIXEL_ARRAY_

#include "Arduino.h"

class AbstractPixelArray {
 public:
  virtual ~AbstractPixelArray() {}
  virtual void Set(int start_idx, int num, const Color3i& c) = 0;
  virtual void Set(int idx, const Color3i& c) = 0;
  virtual void ApplyBlendMultiply(const Color3i& c) = 0;
  virtual int Length() = 0;
  virtual void SetRotationalOffset(int r) = 0;
  virtual void Draw() = 0;
  virtual void Begin() = 0;
  virtual void SetLength(int n) = 0;
  virtual void PaintBlack() = 0;
  
  void SetAll(const Color3i& c) { Set(0, Length(), c); }
  
  virtual Adafruit_NeoPixel* GetStrip() { return NULL; }
};

class NeoPixelArray : public AbstractPixelArray {
 public:
  NeoPixelArray(Adafruit_NeoPixel* neopixel_array) : strip_(neopixel_array) {
    curr_length_ = neopixel_array->numPixels();
    rotation_offset_ = 0;
  }
  
  virtual void Set(int start_idx, int num, const Color3i& c) {
    const int end_idx = start_idx + num;
    
    for (int idx = start_idx; idx < end_idx; ++idx) {
      strip_->setPixelColor(idx, c.r_, c.g_, c.b_); 
    }
  }
  virtual void Set(int idx, const Color3i& c) {
    strip_->setPixelColor(idx, c.r_, c.g_, c.b_);
  }
  virtual void ApplyBlendMultiply(const Color3i& c) {
    const int n = strip_->numPixels();
    for (int i = 0; i < n; ++i) {
      uint8_t* p = strip_->getPixels() + i * 3;
      uint8_t r = (uint16_t(p[0]) * uint16_t(c.r_)) >> 8;
      uint8_t g = (uint16_t(p[1]) * uint16_t(c.g_)) >> 8;
      uint8_t b = (uint16_t(p[2]) * uint16_t(c.b_)) >> 8;
      p[0] = r;
      p[1] = g;
      p[2] = b;
    }
  }
  virtual int Length() { return curr_length_; }
  
  virtual void SetRotationalOffset(int r) { rotation_offset_ = r; }
  virtual void Draw() {
    Rotate(rotation_offset_, strip_);
    strip_->show();
    Rotate(-rotation_offset_, strip_);
  }
  virtual void Begin() { strip_->begin(); }
  
  // Sets all pixels black, regardless of the curr_pixel size. Good for "Turning Off"
  // the strip.
  virtual void PaintBlack() {
    for (int i = 0; i < strip_->numPixels(); ++i) {
      strip_->setPixelColor(i, 0, 0, 0);
    }
  }
  
  virtual void SetLength(int n) { curr_length_ = min(n, strip_->numPixels()); }
  
  virtual Adafruit_NeoPixel* GetStrip() { return strip_; }
  
  
  void Rotate(int m, Adafruit_NeoPixel* strip) {
    const int n = Length();
    if (0 == m)
      return;
    
    if (m > 0) {
      m = m % n;  
    } else {
      m = -m;
      m = m % n;
      m = n - m;
    }

      
    if (0 == m % n)
      return;
      

    
    // These three in-place reversals will effectly rotate the buffer.
    Reverse(0, m, strip);
    Reverse(m, n, strip);
    Reverse(0, n, strip);
  }
  static void Reverse(int begin_n, int end_n, Adafruit_NeoPixel* strip) {  
    if (begin_n == end_n)
      return;
    int last_n = end_n - 1;
    
    while (begin_n < last_n) {
      const uint32_t temp = strip->getPixelColor(begin_n);
      strip->setPixelColor(begin_n, strip->getPixelColor(last_n));
      strip->setPixelColor(last_n, temp);
      ++begin_n;
      --last_n;
    }
  }
  
  
  Adafruit_NeoPixel* strip_;
  int curr_length_;
  int rotation_offset_;
};




////////////////////////////////////////////////////////////////
// Joins multiple strips together to act as one logical strip.
class MultiNeoPixelArray : public AbstractPixelArray {
 public:
  MultiNeoPixelArray(Adafruit_NeoPixel* neopixel_array0,
                     Adafruit_NeoPixel* neopixel_array1) {
    strip_[0] = neopixel_array0;
    strip_[1] = neopixel_array1;
    
    curr_length_ = neopixel_array0->numPixels() + neopixel_array1->numPixels();
    rotation_offset_ = 0;
  }
  
  virtual void Set(int start_idx, int num, const Color3i& c) {
    const int end_idx = start_idx + num;
    
    for (int idx = start_idx; idx < end_idx; ++idx) {
      Set(idx, c); 
    }
  }
  
  void GetStripAndIdx(int idx, Adafruit_NeoPixel** strip_dst, int* idx_dst) {
    int which = (idx < strip_[0]->numPixels()) ? 0 : 1;
    *idx_dst = (which == 0) ? idx : idx - strip_[0]->numPixels();
    *strip_dst = strip_[which];
  }
  
  uint8_t* GetPixel(int idx) {
    Adafruit_NeoPixel* strip;
    int real_idx;
    GetStripAndIdx(idx, &strip, &real_idx);
    uint8_t* out = strip->getPixels() + (real_idx * 3);
    return out;
  }
  
  virtual void Set(int idx, const Color3i& c) {
    uint8_t* p = GetPixel(idx);
    p[0] = c.g_;
    p[1] = c.r_;
    p[2] = c.b_;
  }
  
  void BlendMultiply(int idx, const Color3i& c) {
    uint8_t* p = GetPixel(idx);
    uint8_t r = (uint16_t(p[0]) * uint16_t(c.r_)) >> 8;
    uint8_t g = (uint16_t(p[1]) * uint16_t(c.g_)) >> 8;
    uint8_t b = (uint16_t(p[2]) * uint16_t(c.b_)) >> 8;
    p[0] = r;
    p[1] = g;
    p[2] = b;
  }
  
  virtual void ApplyBlendMultiply(const Color3i& c) {
    const int n = curr_length_;
    for (int i = 0; i < n; ++i) {
      BlendMultiply(i, c);
    }
  }
  virtual int Length() { return curr_length_; }
  
  // Not supported yet...
  virtual void SetRotationalOffset(int r) { rotation_offset_ = r; }
  virtual void Draw() {
    Rotate(rotation_offset_);
    strip_[0]->show();
    strip_[1]->show();
    Rotate(-rotation_offset_);
  }
  virtual void Begin() { strip_[0]->begin(); strip_[1]->begin(); }
  
  // Sets all pixels black, regardless of the curr_pixel size. Good for "Turning Off"
  // the strip.
  virtual void PaintBlack() {
    for (int i = 0; i < Length(); ++i) {
      Set(i, Color3i::Black());
    }
  }
  
  virtual void SetLength(int n) {
    int min_n = strip_[0]->numPixels() + strip_[1]->numPixels();
    
    curr_length_ = min(n, min_n);
  }
  
  
  
  void Rotate(int m) {
    const int n = Length();
    if (0 == m)
      return;
    
    if (m > 0) {
      m = m % n;  
    } else {
      m = -m;
      m = m % n;
      m = n - m;
    }

      
    if (0 == m % n)
      return;
      

    
    // These three in-place reversals will effectly rotate the buffer.
    Reverse(0, m);
    Reverse(m, n);
    Reverse(0, n);
  }
  void Reverse(int begin_n, int end_n) {  
    if (begin_n == end_n)
      return;
    int last_n = end_n - 1;
    
    while (begin_n < last_n) {
      uint8_t* p0 = GetPixel(begin_n);
      uint8_t* p1 = GetPixel(last_n);
      
      // Swap components
      for (int i = 0; i < 3; ++i) {
        uint8_t v = p0[i];
        p0[i] = p1[i];
        p1[i] = v;
      }
      
      ++begin_n;
      --last_n;
    }
  }
  
  
  Adafruit_NeoPixel* strip_[2];
  int curr_length_;
  int rotation_offset_;
};


#endif  // _ABSTRACT_PIXEL_ARRAY_

