/*
 * DriftRiff Mini - Audio Engine Header
 */

#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include <Arduino.h>

#define AUDIO_OUTPUT_PIN    25    // ESP32 internal DAC
#define SAMPLE_RATE         22050 // Hz
#define AUDIO_BUFFER_SIZE   512
#define MAX_CONCURRENT_SAMPLES 4

struct AudioSample {
  uint8_t* data;
  uint32_t size;
  uint32_t position;
  bool active;
  float volume;
};

class AudioEngine {
private:
  AudioSample activeSamples[MAX_CONCURRENT_SAMPLES];
  unsigned long lastSampleTime;
  uint32_t sampleInterval; // microseconds between samples
  uint8_t outputBuffer[AUDIO_BUFFER_SIZE];
  uint16_t bufferPosition;
  bool isInitialized;
  
  void mixSamples();
  uint8_t clipSample(int16_t sample);
  
public:
  AudioEngine();
  
  void init();
  void update();
  void playSample(uint8_t* sampleData, uint32_t sampleSize, float volume = 1.0);
  void stopAllSamples();
  void setMasterVolume(float volume);
  
  bool isPlaying();
};
