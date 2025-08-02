/*
 * DriftRiff Mini - Sequencer Module Header
 */

#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <Arduino.h>

#define NUM_TRACKS 6
#define NUM_STEPS 16
#define MIN_BPM 60
#define MAX_BPM 200
#define DEFAULT_BPM 120

class Sequencer {
private:
  bool steps[NUM_TRACKS][NUM_STEPS];
  int currentStep;
  int bpm;
  bool isRunning;
  
public:
  Sequencer();
  
  void init();
  void nextStep();
  void reset();
  
  // Step control
  bool isStepActive(int track, int step);
  void toggleStep(int track, int step);
  void setStep(int track, int step, bool active);
  void clearTrack(int track);
  void clearAll();
  
  // Playback control
  void togglePlayback();
  void play();
  void pause();
  bool isPlaying();
  
  // BPM control
  void setBPM(int newBPM);
  void increaseBPM();
  void decreaseBPM();
  int getBPM();
  
  // Getters
  int getCurrentStep();
  bool (*getSteps())[NUM_STEPS] { return steps; }
};
