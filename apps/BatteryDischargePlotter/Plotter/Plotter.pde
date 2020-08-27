
 // Graphing sketch
 
 
 // This program takes ASCII-encoded strings
 // from the serial port at 9600 baud and graphs them. It expects values in the
 // range 0 to 1023, followed by a newline, or newline and carriage return
 
 // 
 
 import processing.serial.*;
 
 Serial myPort;        // The serial port
 int xPos = 1;         // horizontal position of the graph
 
 void setup () {
   // set the window size:
   size(1024, 768);        
   
   // List all the available serial ports
   println("Detected ports:");
   println(Serial.list());
   
   // Autodetect serial port for arduino.
   int idx = 0;
   for (int i = 0; i < Serial.list().length; ++i) {
     String name = Serial.list()[i];
     if (name.contains("tty.usbmodem")) {
       idx = i;
       break;
     }
   }
   
   myPort = new Serial(this, Serial.list()[idx], 9600);
   // don't generate a serialEvent() unless you get a newline character:
   myPort.bufferUntil('\n');
   // set inital background:
   background(0);
 }
 void draw () {
   // everything happens in the serialEvent()
 }
 
 void serialEvent (Serial myPort) {
   // get the ASCII string:
   String inString = myPort.readStringUntil('\n');
   
   if (inString != null) {
     inString = trim(inString);
     
     String[] tokens = split(inString, ':');
     
     if (tokens.length < 2) {
       if (1 == tokens.length) {
         // This is a status message.
         println(tokens[0]);
       } else {
         println("UNEXPECTED ERROR: tokens.length() == 0");
       }
       return;
     }
     inString = split(inString, ':')[1];
     
     println(inString);
     // convert to an int and map to the screen height:
     float voltage = float(inString); 
     float inByte = map(voltage, 0, 5.0, 0, height);
   
     // Increase visual quality (maybe)
     smooth(4);
   
     // draw the line:
     stroke(127,34,255);
     line(xPos, height, xPos, height - inByte);
   
     stroke(255, 0, 0);
     float h = (1.0 - 3.3 / 5.0) * height;
     line(0, h, width, h);
     
     stroke(255, 128.0);
     line(0, h, width, h);
     
     stroke(0, 255, 0);
     h = (1.0 - 3.7 / 5.0) * height;
     line(0, h, width, h);
     
     stroke(0, 0, 255);
     h = (1.0 - 4.2 / 5.0) * height;
     line(0, h, width, h);
  
     fill(0, 0, 0);
     stroke(255, 0, 0);
     rect(0, 0, 50, 30);
  
     textSize(16);
     stroke(255, 255, 255);
     fill(255,0,0);
     text("" + voltage + "v", 3, 20); 
   
     // at the edge of the screen, go back to the beginning:
     if (xPos >= width) {
       xPos = 0;
       background(0); 
     } else {
       // increment the horizontal position:
       xPos++;
     }
   }
 }
