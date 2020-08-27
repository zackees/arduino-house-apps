#include "Arduino.h"

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
      Serial.println("RefillBuffer FAILED");
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

