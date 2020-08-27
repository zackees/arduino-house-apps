#include "Arduino.h"

struct Pixel_APA102 {
  static Pixel_APA102 Black() { return Pixel_APA102(0xff, 0x0, 0x0, 0x0); }
  static Pixel_APA102 Vsync() { return Pixel_APA102(0x00, 0x0, 0x0, 0x0); }
  
  Pixel_APA102() : ctr(0xff), r(0xff), g(0xff), b(0xff) {}
  Pixel_APA102(byte _ctr, byte _r, byte _g, byte _b) { ctr = _ctr; r = _r; g = _g; b = _b; }
  bool IsVsync() { return (ctr == 0) && (r == 0) && (g == 0) && (b == 0); }
  void Print() { Serial.print(ctr); Serial.print(','); Serial.print(r); Serial.print(','); Serial.print(g); Serial.print(','); Serial.print(b); Serial.println(""); }
  void PrintByte(byte val) { Serial.print(val); Serial.print(','); }
  byte ctr;
  byte r;
  byte g;
  byte b;
};


class VideoFile {
 public:
  VideoFile() {
    init();
  }
  VideoFile(const char* path) {
    init();
    if (!Open(path)) {
      Serial.print("VideoFile: ERROR IN CONSTRUCTOR - "); Serial.println(path);
      delay(1000);
    }
  }
  

  
  virtual ~VideoFile() {}
  
  bool Open(const char* path) {
    Close();
    mPixelsPerFrame = CalcFrameSize(path);
    return mFileBuffer.Open(SD.open(path));
  }
  
  void Close() {
    mFileBuffer.Close();
    mPixelsPerFrame = 0;
  }
  
  int32_t PixelsPerFrame() { return mPixelsPerFrame; }
  bool ReadPixel(Pixel_APA102* dst) {
    bool ok = true;
    ok = mFileBuffer.Read(&dst->ctr); if (!ok) return false;
    ok = mFileBuffer.Read(&dst->r);   if (!ok) return false;
    ok = mFileBuffer.Read(&dst->g);   if (!ok) return false;
    ok = mFileBuffer.Read(&dst->b);
    return ok;
  }
  
  int32_t FramesRemaining() {
    if (mPixelsPerFrame == 0) { return 0; }
    int32_t out = mFileBuffer.BytesLeft() / mPixelsPerFrame;
    return (out < 0) ? 0 : out;
  }
  
  void Rewind() {
    mFileBuffer.RewindToStart();
  }

  
 private:
  void init() {
    mPixelsPerFrame = 0;
  }
  int32_t CalcFrameSize(const char* fileName) {
    FileBuffer FileBuffer;
    bool ok = FileBuffer.Open(SD.open(fileName));
    if (!ok) return -1;
    // seek until we find a video sync
    const int kMaxCount = 10000;
    
  
    int32_t vsyncIdx = -1;
  
    uint8_t buf[4] = {0};
  
    int loop_cnt = 0;
  
    while(loop_cnt < kMaxCount) {
      
      // Read one pixel.
      for (int i = 0; i < 4; ++i) {
        int32_t val = FileBuffer.read();
        if (val == -1) { ok = false; break; }
        buf[i] = (int8_t)val;
      }
      
      if (buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0) {
        vsyncIdx = loop_cnt;
        break;
      }
  
      loop_cnt++;
    }
    FileBuffer.Close();
    return vsyncIdx;
  }
 
  int32_t mPixelsPerFrame;
  FileBuffer mFileBuffer;
};
