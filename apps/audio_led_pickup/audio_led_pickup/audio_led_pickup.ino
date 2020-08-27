/*
 Fade
 
 This example shows how to fade an led0 on pin 9
 using the analogWrite() function.
 
 This example code is in the public domain.
 */
 
int led0 = 9;           // the pin that the led0 is attached to
int led1 = 10;
int led2 = 11;

int light_sensor = A0;


int delay_speed = 10;



int MaxAmp(int pin, int n_samples) {
  int max_out = 0;
  for (int i = 0; i < n_samples; ++i) {
    int pos_sensor_value = abs(analogRead(pin) - 512) * 2;
    max_out = max(max_out, pos_sensor_value);
  }
  Serial.println(max_out);
  return max_out;
}


int Clamp(int val, int min_val, int max_val) {
  if (val < min_val) {
    return min_val;
  }
  if (val > max_val) {
    return max_val;
  }
  return val;
}

float Clampf(float val, float min_val, float max_val) {
  if (val < min_val) {
    return min_val;
  }
  if (val > max_val) {
    return max_val;
  }
  return val;
}


int MapLedBrightness(int val) {
  float f = static_cast<float>(val) / 255.0f;
  int max_amp = MaxAmp(A5, 100);
  
  float max_amp_ratio =
      static_cast<float>(max_amp) /
      1024.0f;

  max_amp_ratio = Clampf(max_amp_ratio, 0.0f, 1.0f);

  f *= max_amp_ratio;
  int rtn = static_cast<int>(f * 255) ;
  
  
  return rtn > 128 ? 255 : 0;
}

void StepLed(int amount, int led, int* led_val) {
  *led_val = Clamp(*led_val + amount, 0, 255);
  
  int led_final_val = MapLedBrightness(*led_val);

  analogWrite(led, led_final_val);
}

void Increment(int led, int* led_val) {
  StepLed(1, led, led_val);
}

void Decrement(int led, int* led_val) {
  StepLed(-1, led, led_val);
}

void ledOff(int led) {
  analogWrite(led, 0);
}


// the setup routine runs once when you press reset:
void setup()  { 
  // declare pin 9 to be an output:
  pinMode(led0, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(light_sensor, INPUT);
  
  // Allows debugging.
  Serial.begin(9600);
}


// the loop routine runs over and over again forever:
void loop()  {
  for (int i = 0; i < 255; ++i) {
    int led_value = MapLedBrightness(255);
    Serial.println(led_value);
    analogWrite(led0, led_value); 
    delay(100);
  }
}

