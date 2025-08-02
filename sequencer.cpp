/*
 * DriftRiff Mini - Sequencer Module Implementation
 */

#include "sequencer.h"

Sequencer::Sequencer() {
  currentStep = 0;
  bpm = DEFAULT_BPM;
  isRunning = true;
  
  // Initialize all steps to false
  for (int track = 0; track < NUM_TRACKS; track++) {
    for (int step = 0; step < NUM_STEPS; step++) {
      steps[track][step] = false;
    }
  }
}

void Sequencer::init() {
  // Set up some default pattern for demo
  // Track 0 (kick): steps 0, 4, 8, 12
  steps[0][0] = true;
  steps[0][4] = true;
  steps[0][8] = true;
  steps[0][12] = true;
  
  // Track 1 (snare): steps 4, 12
  steps[1][4] = true;
  steps[1][12] = true;
  
  // Track 2 (hihat): every other step
  for (int i = 1; i < NUM_STEPS; i += 2) {
    steps[2][i] = true;
  }
  
  Serial.println("Sequencer initialized with default pattern");
}

void Sequencer::nextStep() {
  if (!isRunning) return;
  
  currentStep++;
  if (currentStep >= NUM_STEPS) {
    currentStep = 0;
  }
}

void Sequencer::reset() {
  currentStep = 0;
}

bool Sequencer::isStepActive(int track, int step) {
  if (track < 0 || track >= NUM_TRACKS || step < 0 || step >= NUM_STEPS) {
    return false;
  }
  return steps[track][step];
}

void Sequencer::toggleStep(int track, int step) {
  if (track < 0 || track >= NUM_TRACKS || step < 0 || step >= NUM_STEPS) {
    return;
  }
  steps[track][step] = !steps[track][step];
  
  Serial.print("Toggled step - Track: ");
  Serial.print(track);
  Serial.print(", Step: ");
  Serial.print(step);
  Serial.print(", Active: ");
  Serial.println(steps[track][step]);
}

void Sequencer::setStep(int track, int step, bool active) {
  if (track < 0 || track >= NUM_TRACKS || step < 0 || step >= NUM_STEPS) {
    return;
  }
  steps[track][step] = active;
}

void Sequencer::clearTrack(int track) {
  if (track < 0 || track >= NUM_TRACKS) return;
  
  for (int step = 0; step < NUM_STEPS; step++) {
    steps[track][step] = false;
  }
  
  Serial.print("Cleared track: ");
  Serial.println(track);
}

void Sequencer::clearAll() {
  for (int track = 0; track < NUM_TRACKS; track++) {
    clearTrack(track);
  }
  Serial.println("Cleared all tracks");
}

void Sequencer::togglePlayback() {
  isRunning = !isRunning;
  Serial.print("Playback ");
  Serial.println(isRunning ? "started" : "paused");
}

void Sequencer::play() {
  isRunning = true;
  Serial.println("Playback started");
}

void Sequencer::pause() {
  isRunning = false;
  Serial.println("Playback paused");
}

bool Sequencer::isPlaying() {
  return isRunning;
}

void Sequencer::setBPM(int newBPM) {
  if (newBPM >= MIN_BPM && newBPM <= MAX_BPM) {
    bpm = newBPM;
    Serial.print("BPM set to: ");
    Serial.println(bpm);
  }
}

void Sequencer::increaseBPM() {
  if (bpm < MAX_BPM) {
    bpm += 5;
    Serial.print("BPM increased to: ");
    Serial.println(bpm);
  }
}

void Sequencer::decreaseBPM() {
  if (bpm > MIN_BPM) {
    bpm -= 5;
    Serial.print("BPM decreased to: ");
    Serial.println(bpm);
  }
}

int Sequencer::getBPM() {
  return bpm;
}

int Sequencer::getCurrentStep() {
  return currentStep;
}
