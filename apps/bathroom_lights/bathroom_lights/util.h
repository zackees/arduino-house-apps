
float CosTime(float hz) {
  unsigned long time_ms = millis();
  time_ms = static_cast<unsigned long>(static_cast<float>(time_ms) * hz);
  const float time_cycle = static_cast<float>(time_ms % 1000) / 1000.0f;
  float cos_val = cos(time_cycle * PI * 2.0f);
  return cos_val;
}

float SinTime(float hz) {
  unsigned long time_ms = millis();
  time_ms = static_cast<unsigned long>(static_cast<float>(time_ms) * hz);
  const float time_cycle = static_cast<float>(time_ms % 1000) / 1000.0f;
  float sin_val = sin(time_cycle * PI * 2.0f);
  return sin_val;
}

float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

