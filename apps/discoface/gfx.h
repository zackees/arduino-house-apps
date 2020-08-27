#include "Arduino.h"

class Gfx {
 public:
  // Implement me to get drawing.
  void OutputPixel(Pixel_APA102 datum) {}
  
  
  // void SetFilter();
  void DrawFrame(VideoFile* vid) {
    const int32_t n = vid->PixelsPerFrame();
    Pixel_APA102 pixel;
    for (int i = 0; i < n; ++i) {
      bool ok = vid->ReadPixel(&pixel);
      if (!ok) { pixel = Pixel_APA102::Black(); }
      
      OutputPixel(pixel);
    }
    EndDraw(vid);
  }
  
  void EndDraw(VideoFile* vid) {
    Pixel_APA102 pixel;
    if (vid->ReadPixel(&pixel)) {
      if (!pixel.IsVsync()) {
        Serial.print(__FUNCTION__); Serial.println(": NO VSYNC");
      }
    } else {
      // Expected vsync pixel. Take recovery action...
      Serial.print(__FUNCTION__); Serial.println(": NO PIXEL");
    }
    OutputPixel(Pixel_APA102::Vsync());
  }
};

