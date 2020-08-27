#include <MIDI.h>

#define dprint(x) Serial.print(x)
#define dprintln(x) Serial.println(x)

// This function will be automatically called when a NoteOn is received.
// It must be a void-returning function with the correct parameters,
// see documentation here: 
// http://arduinomidilib.sourceforge.net/class_m_i_d_i___class.html

void HandleNoteOn(byte channel, byte note, byte velocity) { 
  dprint("HandleNoteOn:");
  
  dprint(", note = ");
  dprint(note);
  
  dprint(", velocity = ");
  dprintln(velocity);
}

void HandleAfterTouchPoly(byte channel, byte note, byte pressure) { 
  dprint("HandleAfterTouchPoly:");

  dprint("\tnote = ");
  dprint(note);

  dprint(", pressure = ");
  dprintln(pressure);
}

void setup() {
  Serial.begin(9600);
  // Initiate MIDI communications, listen to all channels
  MIDI.begin(/*MIDI_CHANNEL_OMNI*/);    
  
  // Connect the HandleNoteOn function to the library, so it is called upon reception of a NoteOn.
  MIDI.setHandleNoteOn(HandleNoteOn);  // Put only the name of the function
  MIDI.setHandleAfterTouchPoly(HandleAfterTouchPoly);
  
}


void loop() {
  // Call MIDI.read the fastest you can for real-time performance.
  MIDI.read();
//  Serial.println("hello world");
  // There is no need to check if there are messages incoming if they are bound to a Callback function.
}
