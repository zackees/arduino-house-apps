

float spring_pos = 0;
float spring_vel = 0;
float spring_mass = .5;
float spring_stiffness = 1;
float spring_drag_coefficient = .5;

void ResetSpring() {
    spring_pos = 0;
    spring_vel = 0;
}

float CalcSpringPosition(float applied_force, float delta_time_sec) {
  float spring_drag_force = (spring_vel * spring_vel) *
                            (spring_drag_coefficient) *
                            (spring_vel > 0 ? -1.f : 1.f);
                            
  float spring_force = spring_stiffness * -spring_pos;
  float force = applied_force + spring_force + spring_drag_force;

  float delta_v = force * delta_time_sec / spring_mass;
  spring_vel += delta_v;
  spring_pos += spring_vel * delta_time_sec;
  
  if (fabs(spring_pos) > 1.0f) {
    spring_pos = (spring_pos > 0.0f ? 1.0 : -1.0);
    spring_vel = 0;
  }
  
  return spring_pos;
}

float DeltaTime() {
  static unsigned long s_prev_time = millis();
  unsigned long now = millis();
  unsigned long time_delta = now - s_prev_time;
  s_prev_time = now;
  float output = time_delta;
  return output / 1000.f; 
}
