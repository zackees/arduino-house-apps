#define MIC_PIN A5


void setup() {
  Serial.begin(9600);
  pinMode(MIC_PIN, INPUT);
}

void PrintVolume() {
  int min_val, max_val;
  min_val = max_val = analogRead(MIC_PIN);
  for (int i = 0; i < (2 << 4); ++i) {
    int val = analogRead(MIC_PIN);
    min_val = min(min_val, val);
    max_val = max(max_val, val);
  }
  
  int volume = max_val - min_val;
  Serial.println(volume);
}

void loop() {
  PrintVolume();
  //Serial.println(analogRead(MIC_PIN));
  //delay(16);  // 60fps
}
