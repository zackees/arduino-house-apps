
 // Graphing sketch
 
 
 // This program takes ASCII-encoded strings
 // from the serial port at 9600 baud and graphs them. It expects values in the
 // range 0 to 1023, followed by a newline, or newline and carriage return
 
 // 
 
 import processing.serial.*;
 
 Serial myPort;        // The serial port
 
 FloatList data;
 
 int h = 480;
 int w = 1000;
 boolean movie_maker = false;
 
 PGraphics gfx, offscreen_gfx;
 
 Serial FindSerialPort() {
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
   
   return new Serial(this, Serial.list()[idx], 57600);
 }
 
 void setup () {
   // set the window size:
   size(w, h);
   // Create an off-screen buffer.
   gfx = createGraphics(w, h, JAVA2D);
   offscreen_gfx = createGraphics(w, h, JAVA2D);
   data = new FloatList();

   frameRate(30);
   myPort = FindSerialPort();
   
   // List all the available serial ports

   // don't generate a serialEvent() unless you get a newline character:
   myPort.bufferUntil('\n');
   // set inital background:
   background(0);
 }
 
 void DrawVoltageLine(PGraphics buffer, float volt, String str, int r, int g, int b) {
   buffer.stroke(r, g, b);
   float h = (1.0 - volt / 5.0) * height;
   buffer.line(0, h, width, h);
   buffer.textSize(12);
   buffer.fill(r,g,b);
   buffer.text(str, 0, h - 2);
 }
 
int s_cooldown_until = 0; 
 
 void draw () {

   if (keyPressed && (s_cooldown_until < millis())) {
     if (key == 'm' || key == 'M') {
       movie_maker = !movie_maker;
       s_cooldown_until = millis() + 400;
     }
   }
   DrawFrame(gfx);
   PImage img = gfx.get(0, 0, gfx.width, gfx.height);
   image(img, 0, 0);
 }
 
 void DrawFrame(PGraphics buffer) {
   buffer.beginDraw();
   buffer.background(0);
   buffer.smooth();
   //background(0);
   // Increase visual quality (maybe)
   //smooth(4);
   


   for (int i = 0; i < data.size(); ++i) {
     buffer.stroke(127,34,255);
     float inByte = map(data.get(i), 0, 5.0, 0, height);
     buffer.line(i, height, i, height - inByte);
     if (0 == i % 10) {
       buffer.stroke(0,0,0);
       buffer.line(i, 0, i, height);
     }
   }

   DrawVoltageLine(buffer, 3.3, "3.3v", 255, 0, 0);
   DrawVoltageLine(buffer, 4.2, "4.2v", 0, 0, 255);
   DrawVoltageLine(buffer, 3.7, "3.7v", 0, 255, 0);
   DrawVoltageLine(buffer, 3.0, "3.0v", 255, 255, 0);
   DrawVoltageLine(buffer, 2.5, "2.5v", 0, 255, 255);
   

   if (data.size() > 0) {
     textSize(16);
     stroke(255, 255, 255);
     fill(255,0,0);
     text("" + data.get(data.size()-1) + "v", 3, 20);
   }
   
   //if (save_to_disk) {
   //  saveFrame("battery_discharge-######.png");
   //}
   
   int r,g,b;
   r = g = b = 255;
   buffer.stroke(r, g, b);
   float h = height - 20;
   buffer.textSize(12);
   buffer.fill(r,g,b);
   if (movie_maker) {
     buffer.text("[M]ovie maker on", 0, h - 2);
   } else {
     buffer.text("[M]ovie maker off", 0, h - 2);
   }
   buffer.endDraw();
 }
 
 void serialEvent (Serial myPort) {
   // get the ASCII string:
   String inString = myPort.readStringUntil('\n');
   HandleData(inString);
 }
 
 int counter = 0;
 void HandleData (String inString) {
   if (inString == null)
     return;
   
   inString = trim(inString);
   
   if (inString.equals("BatteryStressTest-cycle")) {
     if (data.size() > 0 && movie_maker) {
       DrawFrame(offscreen_gfx);  // Saves to disk.
       String s = "movie_stitch/battery_discharge-" + nf(counter++, 6) + ".png";
       println(s);
       offscreen_gfx.save(s);
     }
     data.clear();
   }
   
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
   data.append(voltage);
   while ((data.size() > width) && (data.size() > 0)) {
     data.remove(0);
   }
 }
