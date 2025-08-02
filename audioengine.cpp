/*
 * DriftRiff Mini - Audio Engine Implementation
 */

#include "audioengine.h"

AudioEngine::AudioEngine() {
  lastSampleTime = 0;
  sampleInterval = 1000000 / SAMPLE_RATE; // microseconds
  bufferPosition = 0;
  isInitialized = false;
  
  // Initialize sample slots
  for (int i = 0; i < MAX_CONCURRENT_SAMPLES; i++) {
    activeSamples[i].data = nullptr;
    activeSamples[i].size = 0;
    activeSamples[i].position = 0;
    activeSamples[i].active = false;
    activeSamples[i].volume = 1.0;
  }
}

void AudioEngine::init() {
  // Configure LEDC for PWM audio output
  ledcSetup(0, SAMPLE_RATE * 256, 8); // Channel 0, frequency, resolution
  ledcAttachPin(AUDIO_OUTPUT_PIN, 0);
  
  // Set initial output to mid-level (128 for 8-bit)
  ledcWrite(0, 128);
  
  isInitialized = true;
  Serial.println("Audio engine initialized");
  Serial.print("Sample rate: ");
  Serial.print(SAMPLE_RATE);
  Serial.println(" Hz");
  Serial.print("Sample interval: ");
  Serial.print(sampleInterval);
  Serial.println(" microseconds");
}

void AudioEngine::update() {
  if (!isInitialized) return;
  
  unsigned long currentTime = micros();
  
  // Check if it's time for next sample
  if (currentTime - lastSampleTime >= sampleInterval) {
    lastSampleTime = currentTime;
    
    // Mix all active samples
    mixSamples();
    
    // Output current buffer position
    if (bufferPosition < AUDIO_BUFFER_SIZE) {
      ledcWrite(0, outputBuffer[bufferPosition]);
      bufferPosition++;
    } else {
      bufferPosition = 0;
    }
  }
}

void AudioEngine::playSample(uint8_t* sampleData, uint32_t sampleSize, float volume) {
  if (!sampleData || sampleSize == 0) return;
  
  // Find available sample slot
  for (int i = 0; i < MAX_CONCURRENT_SAMPLES; i++) {
    if (!activeSamples[i].active) {
      activeSamples[i].data = sampleData;
      activeSamples[i].size = sampleSize;
      activeSamples[i].position = 0;
      activeSamples[i].active = true;
      activeSamples[i].volume = volume;
      
      Serial.print("Started sample in slot ");
      Serial.print(i);
      Serial.print(", size: ");
      Serial.println(sampleSize);
      return;
    }
  }
  
  Serial.println("Warning: No available sample slots");
}

void AudioEngine::mixSamples() {
  // Clear buffer section
  for (int i = 0; i < min(32, AUDIO_BUFFER_SIZE - bufferPosition); i++) {
    outputBuffer[bufferPosition + i] = 128; // 8-bit mid-level
  }
  
  // Mix active samples
  for (int slot = 0; slot < MAX_CONCURRENT_SAMPLES; slot++) {
    if (!activeSamples[slot].active) continue;
    
    AudioSample* sample = &activeSamples[slot];
    
    // Process a small chunk of samples
    for (int i = 0; i < min(32, AUDIO_BUFFER_SIZE - bufferPosition); i++) {
      if (sample->position >= sample->size) {
        // Sample finished
        sample->active = false;
        Serial.print("Sample finished in slot ");
        Serial.println(slot);
        break;
      }
      
      // Get sample value and apply volume
      uint8_t sampleValue = sample->data[sample->position];
      int16_t scaledValue = (int16_t)((sampleValue - 128) * sample->volume) + 128;
      
      // Mix with existing buffer (simple addition with clipping)
      int16_t mixed = outputBuffer[bufferPosition + i] + (scaledValue - 128);
      outputBuffer[bufferPosition + i] = clipSample(mixed + 128);
      
      sample->position++;
    }
  }
}

uint8_t AudioEngine::clipSample(int16_t sample) {
  if (sample < 0) return 0;
  if (sample > 255) return 255;
  return (uint8_t)sample;
}

void AudioEngine::stopAllSamples() {
  for (int i = 0; i < MAX_CONCURRENT_SAMPLES; i++) {
    activeSamples[i].active = false;
    activeSamples[i].position = 0;
  }
  
  // Reset output to mid-level
  ledcWrite(0, 128);
  
  Serial.println("All samples stopped");
}

void AudioEngine::setMasterVolume(float volume) {
  // Apply volume to all active samples
  for (int i = 0; i < MAX_CONCURRENT_SAMPLES; i++) {
    if (activeSamples[i].active) {
      activeSamples[i].volume = volume;
    }
  }
}

bool AudioEngine::isPlaying() {
  for (int i = 0; i < MAX_CONCURRENT_SAMPLES; i++) {
    if (activeSamples[i].active) {
      return true;
    }
  }
  return false;
}
