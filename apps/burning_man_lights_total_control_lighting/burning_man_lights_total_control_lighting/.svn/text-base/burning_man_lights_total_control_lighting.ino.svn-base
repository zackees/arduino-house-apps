#include <Arduino.h>

#include <Coroutine.h>
#include <CircularArray.h>
#include <SPI.h>
#include <TCL.h>
#include <LedRopeTCL.h>
#include <NewDeleteExt.h>
#include <Sineseza.h> 
#include <Vector.h>

#ifndef Arraysize
# define ArraySize(X) (sizeof(X) / sizeof(*X))
#endif // ArraySize

class BeatDetectorCoroutine : public AbstractCoroutine {
 public:
  BeatDetectorCoroutine(Sineseza* freq_sampler)
    : freq_sampler_(freq_sampler), last_low_beat_(0) {
    beats_size_ = Sineseza::Output::spectrum_array_size;
    beats_ = new bool[beats_size_];
    while (last_freq_sample_.size() < beats_size_) {
      last_freq_sample_.push_back(0);
      last_freq_sign_.push_back(false);
    }
  }
  
  virtual ~BeatDetectorCoroutine() {
    delete[] beats_;
  }
  
  int Intensity(int frequency_index) {
    return last_freq_sample_[frequency_index];
  }
    
  virtual int OnCoroutine() {
    Sineseza::Output freq_output;
    freq_sampler_->ProcessAudioTo(&freq_output);
    freq_sampler_->PrintBands(freq_output);
    
    for (int i = 0; i < beats_size_; ++i) {
      // Is the sound getting more intense since the last sample?
      bool sign_positive = freq_output.spectrum_array[i] > last_freq_sample_[i];
      int freq_intensity = freq_output.spectrum_array[i];
      
      // A local maxima is when the sound begins to fall.
      const bool local_maxima = !sign_positive && last_freq_sign_[i];
      last_freq_sign_[i] =  sign_positive;  // Now save the state.
      
      if (local_maxima && freq_intensity > 64) {
        Serial.print("beat detected[");
        Serial.print(i);
        Serial.println("]");
        beats_[i] = true;
      }
      last_freq_sample_[i] = freq_output.spectrum_array[i];
    }
    
    return 15;  // Sample again in 20 ms.
  }
  
 
  bool ReleasePendingBeat(int index) {
    if (index > beats_size_)
      return false;
    bool val = beats_[index];
    beats_[index] = false;
    return val;
  }
  
  int beats_size() const { return beats_size_; }
    
 private:
  Sineseza* freq_sampler_;
  bool* beats_;
  int beats_size_;
  int last_low_beat_;
  
  Vector<int> last_freq_sample_;
  Vector<bool> last_freq_sign_;
};

///////////////////////////////////////////////////////////////////////////////
class LEDRopeVisualizerParticleThump : public AbstractCoroutine {
 public:
  LEDRopeVisualizerParticleThump(LedRopeTCL* rope_led)
      : rope_led_(rope_led) {
      
  }
    
  virtual ~LEDRopeVisualizerParticleThump() {
  }

  virtual int OnCoroutine() {
    Vector<Particle*>& active_particles = particles_.active_particles();
    rope_led_->ApplyBlendSubtract(Color3i(16, 16, 16));
    for (int i = 0; i < active_particles.size(); ++i) {
      Particle* p = active_particles[i];
      rope_led_->Set(p->position_index, p->color);
      p->position_index++;
      if (p->position_index >= rope_led_->length()) {
        particles_.RetireActiveParticle(p);
      } else {
        ++i;
      }
    }
    return 15;  // Sample again in 20 ms.
  }
  
 
  void OnLowBeat() {
    Particle* p = particles_.OnActivateParticle();
    if (p) {
      p->color.Set(random(0xff), random(0xff), random(0xff));
    }
  }
  
 private:
  typedef LedRopeTCL::Color3i Color3i;
  LedRopeTCL* rope_led_;
  struct Particle {
    Particle() { Reset(); }
    void Reset() { position_index = 0; color.Set(0,0,0); }
    Color3i color;
    int position_index;
  };
  
 
  class ParticleContainer {
   public:
    static const int kNumParticles = 8;
    
    ParticleContainer() {
      for (int i = 0; i < kNumParticles; ++i) {
        available_particles_.push_back(new Particle);
      }
    }
    
    ~ParticleContainer() {
      for (int i = 0; i < available_particles_.size(); ++i) {
        delete available_particles_[i];
      }
      available_particles_.clear();
      
      for (int i = 0; i < active_particles_.size(); ++i) {
        delete active_particles_[i];
      }
      active_particles_.clear();
    }
    
    Particle* OnActivateParticle() {
      Particle* output = NULL;
      if (available_particles_.size()) {
        output = available_particles_.back();
        output->Reset();
        available_particles_.pop_back();
        active_particles_.push_back(output);
      }
      return output;
    }
    
    void RetireActiveParticle(Particle* p) {
      int index_active = -1;
      active_particles_.erase(p);
      available_particles_.push_back(p);
    }
    
    Vector<Particle*>& active_particles() {
      return active_particles_;
    }
    
   private:
    Vector<Particle*> available_particles_;
    Vector<Particle*> active_particles_;
  };

  ParticleContainer particles_;
};


// 100 leds.
LedRopeTCL rope_led(100);
Sineseza sound_freq_sampler;  // Use default shield pins.

BeatDetectorCoroutine beat_detector(&sound_freq_sampler);
LEDRopeVisualizerParticleThump led_rope_visualizer(&rope_led);
// Put all routines in here so that the routine_dispatcher picks it up.
AbstractCoroutine* routines[] = { &beat_detector, &led_rope_visualizer };

CoroutineDispatch routine_dispatcher(routines);
 
void setup() {
  Serial.begin(9600);
}

void loop() {
  // Run the dispatcher.
  int delay_ms = routine_dispatcher.Update();
  
  if (beat_detector.ReleasePendingBeat(2)) {
    led_rope_visualizer.OnLowBeat();
  }
  // Did the beat detector find a recent beat?
  if (beat_detector.ReleasePendingBeat(0)) {
    rope_led.ApplyBlendAdd(LedRopeTCL::Color3i(64, 0, 0));
  }
  
  rope_led.Draw();
  delay(delay_ms);
  
  //Serial.print("delay_ms ");
  //Serial.println(delay_ms);
}





