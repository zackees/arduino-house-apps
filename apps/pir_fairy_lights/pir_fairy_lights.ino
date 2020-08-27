
#define LED_PIN 3
#define PIR_PIN 8

// The leds can trip the sensor back on. This value is a cooloff
// to prevent hysterisis.
#define SENSOR_ON_COOLOFF_MS 500ul

// This value defines how long the sensor stays on once it's been tripped.
#define SENSOR_ON_DURATION_MS (1000ul * 60ul * 2ul)  // 2 mins

// 0 : Full brightness
// 1 : Half brightness
// 2 : Quarter Brightness
// 3 : Eight Brightness ...
#define INTENSITY_SHIFT 1

// Uncomment the following line to have debug tracing.
//#define DB_PRINT

#ifdef DBG_PRINT
# define DbgPrint(X) Serial.print(X)
# define DbgPrintln(X) Serial.println(X)
#else
# define DbgPrint(X) (X)
# define DbgPrintln(X) (X)
#endif

#define DbgPrint(X) Serial.print(X)
#define DbgPrintln(X) Serial.println(X)

class PIRSensor {
 public:
 
  explicit PIRSensor(int pin)
      : pir_pin_(pin),
        on_(false),
        timestamp_on_(0) {
    //pinMode(pir_pin_, INPUT_PULLUP);
  }
    
  void Update() {
    
    boolean prev_on = on_;
    boolean on = (HIGH == digitalRead(pir_pin_));
    
    unsigned long now_time = millis();
    // Filter the on state. The lights turning off will trip the
    // PIR into coming back on.
    
    DbgPrintln(on ? "1" : "0");
    
    
    if (timestamp_off_ != 0) {
      unsigned long time_off_delta = now_time - timestamp_off_;
      
      // Filter for at least a second.
      if (time_off_delta < SENSOR_ON_COOLOFF_MS) {
         DbgPrint("time_off_delta: "); DbgPrintln(time_off_delta);
         on_ = false;
         return;
       }
    }
    
    if (timestamp_on_ != 0) {
      if (on) {
        timestamp_on_ = now_time;
      }
      
      unsigned long time_on_delta = now_time - timestamp_on_;
      
      // Filter for at least a second.
      if (time_on_delta < SENSOR_ON_DURATION_MS) {
         DbgPrint("time_on_delta: "); DbgPrintln(time_on_delta);
         on_ = true;
         return;
       }
    }
    
    
    if (on) {
      timestamp_off_ = 0;
      // Okay, not filter to so turn on.
      timestamp_on_ = now_time; 
    } else {
      
      if (on != prev_on) {
        timestamp_off_ = now_time;
      }
      timestamp_on_ = 0;

    }
    
    on_ = on;
    
  }
    
  bool On() const {  return on_; }
 private:
  int pir_pin_;
  boolean on_;
  unsigned long timestamp_on_;
  unsigned long timestamp_off_;
};



void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  
}

PIRSensor pirSensor(PIR_PIN);
int curr_led_value = 0;




void loop() {
  
  pirSensor.Update();

  bool on = pirSensor.On();
  curr_led_value += on ? 1 : -1;
  DbgPrintln((on ? "ON" : "OFF"));
  curr_led_value = constrain(curr_led_value, 0, (0xff >> INTENSITY_SHIFT));
  
  analogWrite(LED_PIN, curr_led_value);
}
