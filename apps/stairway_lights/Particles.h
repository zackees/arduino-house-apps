
#if !defined(__PARTICLES_H_) && 0
#define __PARTICLES_H_

#include <Arduino.h>
#include "Vector.h"

class Particle {
 public:
  Particle() { Set(0,0); }
  void Set(float p, float v) {
    position_ = p;
    velocity_ = v;
  }
  float velocity_;
  float position_;
};

class ParticlePool {
 public:
  ParticlePool() : all_pool_(kNumParticles),
                   alive_(kNumParticles),
                   recycle_(kNumParticles) {}
  Particle* TryMakeAlive() {
    if (recycle_.empty()) {
      return NULL;
    }
    Particle* particle = recycle_.back();
    recycle_.pop_back();
    alive_.push_back(particle);
    return particle;
  }
  
  void MarkDead(Particle* p) {
    // Find particle in the the alive list. 
    alive_.erase(p);
    recycle_.push_back(p);
  }

  static const int kNumParticles = 10;
  Vector<Particle> all_pool_;
  Vector<Particle*> alive_;
  Vector<Particle*> recycle_;
};

class ParticleGraphics : AbstractCoroutine {
 public:
  ParticleGraphics(ParticlePool* p) : particles_(p) {}
  
  static float Velocity() { return .1; }
  
  void OnSenseTop() {
    if (Particle* p = particles_->TryMakeAlive()) {
      // Set at position 1 with velocity moving toward 0.
      p->Set(1, -Velocity());
    }
  }
  
  void OnSenseBottom() {
    if (Particle* p = particles_->TryMakeAlive()) {
      // Set at position 0 with velocity moving toward 1.
      p->Set(0, Velocity()); 
    }
  }
  
  virtual int OnCoroutine() {
    for (int i = 0; i < particles_->alive_.size();) {
      Particle* p = particles_->alive_[i];
      p->position_ += p->velocity_;
      if (p->position_ > 1.0 || p->position_ < 0.0) {
        particles_->MarkDead(p);
      } else {
        ++i;
      }
    }
    
    return 8;  // Wait 8 Millisecond before running again.
  }
  
  ParticlePool* particles_;
};

#endif  // __PARTICLES_H_

