// these constants describe the pins. They won't change:
const int xpin = A0;                  // x-axis of the accelerometer
const int ypin = A2;                  // y-axis
const int zpin = A1;                  // z-axis (only on 3-axis models)


void ReadAccelerometer(float* xout, float* yout, float* zout) {
  
  
   int x = analogRead(xpin);
   //
   //add a small delay between pin readings.  I read that you should
   //do this but haven't tested the importance
   delay(1); 
   //
   int y = analogRead(ypin);
   //
   //add a small delay between pin readings.  I read that you should
   //do this but haven't tested the importance
   delay(1); 
   //
   int z = analogRead(zpin);
   //
   //zero_G is the reading we expect from the sensor when it detects
   //no acceleration.  Subtract this value from the sensor reading to
   //get a shifted sensor reading.
   float zero_G_x =502; 
   float zero_G_yz = 512;
   //
   //scale is the number of units we expect the sensor reading to
   //change when the acceleration along an axis changes by 1G.
   //Divide the shifted sensor reading by scale to get acceleration in Gs.
   float scale =102.3;
   
   if (xout) { *xout = ((float)x - zero_G_x)/scale;  }
   if (yout) { *yout = ((float)y - zero_G_yz)/scale; }
   if (zout) { *zout = ((float)z - zero_G_yz)/scale; }
}

struct StartupAccelerometer {
  StartupAccelerometer() {
       //Make sure the analog-to-digital converter takes its reference voltage from
     // the AREF pin
     pinMode(xpin, INPUT);
     pinMode(ypin, INPUT);
     pinMode(zpin, INPUT);
  }
  
} on_startup ;
