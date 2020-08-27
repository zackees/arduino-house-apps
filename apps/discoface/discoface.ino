/*
  SD card read/write

 This example shows how to read and write data to and from an SD card file
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4

 */
#define SD_CS_PIN SS
#include <SPI.h>
#include <SdFat.h>

#include "FrameRateLock.h"

// ****
// * Sd card needs to be declared before the following headers.
SdFat SD;

#include "filebuffer.h"
#include "videofile.h"
#include "gfx.h"

Gfx gfx;

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  

  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output
  // or the SD library functions will not work.
  pinMode(10, OUTPUT);

  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  digitalWrite(10, HIGH);
  delay(1);
}


FrameRateLock fps(60.0f);

void loop()
{
  
  VideoFile vids[] = {VideoFile("video.dat"), VideoFile("video2.dat"), VideoFile("video3.dat") };
  int nVids = 3;
  
  const int32_t n = vids[0].PixelsPerFrame();
  Serial.print("Frame size: "); Serial.println(n);
  
  int vidIdx = 0;
  while (true) {
    if (Serial.available()) {
      byte c = Serial.read() - '0';
      vidIdx = constrain(c, 0, nVids - 1);
    }
    
    VideoFile* vid = &vids[vidIdx];
    
    
    Serial.print(vidIdx); Serial.print(" is drawing Frame: "); Serial.println(vid->FramesRemaining());
    unsigned long start_t = millis();
    gfx.DrawFrame(vid);
    unsigned long delta_t = millis() - start_t;
    Serial.print(delta_t); Serial.println("ms");
    
    // Synchronizes the frame.
    fps.Wait();
    if (vid->FramesRemaining() == 0) { vid->Rewind(); Serial.println("vid->Rewind()"); }
  }
}


