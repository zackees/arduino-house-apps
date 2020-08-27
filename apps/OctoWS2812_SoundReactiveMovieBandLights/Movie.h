#define SD_CS_PIN 3  //  Teeensy 3.1 + OctoWs2811


#include "Arduino.h"



void Movie_Setup();
// Returns true if the strip array was written, false otherwise.
void FillWithMovie(unsigned long time_now, int i, Gfx_Strip* strip);



//// --- IMPL --- ////

struct Pixel_APA102 {
  static Pixel_APA102 Black() { return Pixel_APA102(0xff, 0x0, 0x0, 0x0); }
  static Pixel_APA102 Vsync() { return Pixel_APA102(0x00, 0x0, 0x0, 0x0); }
  
  Pixel_APA102() : ctr(0xff), r(0xff), g(0xff), b(0xff) {}
  Pixel_APA102(byte _ctr, byte _r, byte _g, byte _b) { ctr = _ctr; r = _r; g = _g; b = _b; }
  bool IsVsync() const { return (ctr == 0) && (r == 0) && (g == 0) && (b == 0); }
  void Print() { DPRINT(ctr); DPRINT(','); DPRINT(r); DPRINT(','); DPRINT(g); DPRINT(','); DPRINT(b); DPRINTLN(""); }
  void PrintByte(byte val) { DPRINT(val); DPRINT(','); }
  byte ctr;
  byte r;
  byte g;
  byte b;
};


// Yes this class accesses static objects. Please change to reference
// if you want more decoupling.
class FileBuffer {
 public:
  FileBuffer() {
    ResetBuffer();
  }
  virtual ~FileBuffer() { Close(); }

  bool Open(File file) {
    Close();
    mFile = file;
    if (mFile) {
      mIsOpen = true;
      return true;
    }
    return false;
  }

  void Close() {
    if (mFile) {
      mFile.close();
    }
    ResetBuffer();
  }
  
  void RewindToStart() {
    mFile.rewind();
    RefillBuffer();
  }
  
  bool available() {
    return (mIsOpen) && ((mCurrIdx != mLength) || mFile.available());
  }
  
  int32_t BytesLeft() {
    if (!available()) { return -1; }
    int32_t val = mFile.fileSize() - mFile.curPosition();
    return val;
  }
  
  int32_t FileSize() {
    if (!available()) { return -1; }
    return mFile.fileSize();
  }


  // Reads the next byte, else -1 is returned for end of buffer.
  int16_t read() {
    if (!mIsOpen) { return -1; }

    RefillBufferIfNecessary();
    if (mCurrIdx == mLength) {
      return -1;
    }
    
    // main case.
    byte output = mBuffer[mCurrIdx++];
    return output;
  }

  bool Read(uint8_t* dst) {
    int16_t val = read();
    if (val == -1) { return false; }
    
    *dst = (uint8_t)(val & 0xff);
    return true;
  }

 private:

  void ResetBuffer() {
      mIsOpen = false;
      mLength = -1;
      mCurrIdx = -1;
  }



  void RefillBufferIfNecessary() {
    if ((mCurrIdx == mLength) && mFile.available()) {
      RefillBuffer();
    }
  }
  
  void RefillBuffer() {
    if (!mFile.available()) {
      DPRINTLN("RefillBuffer FAILED");
    } else {
      // Needs more buffer yo.
      mLength = mFile.read(mBuffer, kBufferSize);
      mCurrIdx = 0;
    }
  }

  static const int kBufferSize = 512;
  byte mBuffer[kBufferSize];
  int16_t mCurrIdx;
  int16_t mLength;

  File mFile;
  bool mIsOpen;
};


class VideoStream {
 public:
  VideoStream() {
    init();
  }
  VideoStream(const char* path) {
    init();
    if (!Open(path)) {
      DPRINT("VideoStream: ERROR IN CONSTRUCTOR - "); DPRINTLN(path);
      delay(1000);
    }
  }
  
  virtual ~VideoStream() {}
  
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


// This float time starts at 0 and then progresses to 1.0f in
// durationMs.
struct InterpTimer {
  InterpTimer() {
    mIsRunning = false;
    mStartTimeMs = 0;
    mDurationMs = 99999ul;
  }
  void Start(unsigned long durationMs) {
    mStartTimeMs = millis();
    mDurationMs = durationMs;
    mIsRunning = true;
  }
  
  void Stop() {
    mIsRunning = false;
  }
  
  bool IsRunning() { return mIsRunning; }
  
  double value() {
    unsigned long delta_t = millis() - mStartTimeMs;
    double out = double(delta_t) / double(mDurationMs);
    if (out > 1.0) { out = 1.0f; }
    if (out < 0.0) { out = 0.0; }
    return out;
  }
  
  unsigned long mStartTimeMs;
  unsigned long mDurationMs;
  bool mIsRunning;
};



VideoStream* s_videos = NULL;

void Movie_Setup() {

  if (!SD.begin(SD_CS_PIN)) {
    DPRINTLN("initialization failed!");
    return;
  }

  static VideoStream s_vid[] = {
    "video0.dat",
    "video1.dat",
    "video2.dat",
    "video3.dat"
  };

  s_videos = s_vid;
}

static void CheckVsync(VideoStream* vid) {
  Pixel_APA102 pixel;
  if (vid->ReadPixel(&pixel)) {
    if (!pixel.IsVsync()) {
      DPRINT(__FUNCTION__); DPRINTLN(": NO VSYNC");
    }
  } else {
    // Expected vsync pixel.
    DPRINT(__FUNCTION__); DPRINTLN(": NO PIXEL READ FROM VIDEO");
  }
}

void FillWithMovie(unsigned long time_now, int vid_idx, Gfx_Strip* strip) {
  // Early out if this hasn't been setup yet.
  if ((s_videos == NULL) || (vid_idx >= N_AUDIO_CHANNELS)) { SP(__FUNCTION__); SPLN(": error"); return; }

  VideoStream& vid = s_videos[vid_idx];
  if (vid.FramesRemaining() == 0) { vid.Rewind(); }
  if (vid.FramesRemaining() == 0) {
    SP("Error, video ");
    SP(vid_idx);
    SPLN(" is empty and could not be rewound");
    return;
  }

  const int32_t n = vid.PixelsPerFrame();
  for (int i = 0; i < n; ++i) {
    Pixel_APA102 pixel;
    bool ok = vid.ReadPixel(&pixel);
    if (!ok) { pixel = Pixel_APA102::Black(); }

    // Skip this if we would overwrite the strip of leds array.
    if (i >= N_PIXELS_PER_STRIP) { continue; }

    Gfx_Pixel gpixel = {Gfx_Gamma(pixel.r), Gfx_Gamma(pixel.g), Gfx_Gamma(pixel.b)};
    strip->pixels[i] = gpixel;
  }
  CheckVsync(&vid);
}

