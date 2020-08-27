
#ifndef _LENGTH_SELECTION_STATE_
#define _LENGTH_SELECTION_STATE_

#include "Arduino.h"

// TODO: Make this more user friendly.
class LengthSelectionUI {
 public:
  LengthSelectionUI(TwoWireButton* up_btn,
                    TwoWireButton* down_btn)
    : more_pixels_button_(up_btn), less_pixels_button_(down_btn) {
  }
  void Run(CRGB* array) {
    #if 0
    int change = more_pixels_button_->Value() ? 1 : -1;
    

    array->SetLength(array->Length() + change);
    
    // Setting is saved to permanent storage.
    SavedState::Instance().SetPixelCount(array->Length());
    
    array->PaintBlack();
    array->Draw();
    
    if (array->Length() > 0) {
      array->Set(0, array->Length() - 1, Color3i::Red());
      array->Set(array->Length() - 1, Color3i::White());
      array->Draw();
      delay(100);
      return;
    }
    #endif
  }
  
  TwoWireButton* more_pixels_button_;
  TwoWireButton* less_pixels_button_;
};


#endif  // _LENGTH_SELECTION_STATE_
