/*
 * DriftRiff Mini - SD Card Sample Loader Header
 */

#ifndef SDLOADER_H
#define SDLOADER_H

#include <Arduino.h>
#include <SD.h>

#define MAX_SAMPLE_SIZE     32768  // 32KB max per sample
#define NUM_SAMPLE_SLOTS    6      // One per track

class SDLoader {
private:
  uint8_t* sampleData[NUM_SAMPLE_SLOTS];
  uint32_t sampleSizes[NUM_SAMPLE_SLOTS];
  bool samplesLoaded[NUM_SAMPLE_SLOTS];
  
  const char* sampleFiles[NUM_SAMPLE_SLOTS] = {
    "/samples/kick.raw",
    "/samples/snare.raw", 
    "/samples/hihat.raw",
    "/samples/perc.raw",
    "/samples/bass.raw",
    "/samples/lead.raw"
  };
  
  bool loadSample(int slot, const char* filename);
  void freeSample(int slot);
  
public:
  SDLoader();
  ~SDLoader();
  
  void init();
  bool loadAllSamples();
  void unloadAllSamples();
  
  uint8_t* getSampleData(int slot);
  uint32_t getSampleSize(int slot);
  bool isSampleLoaded(int slot);
  
  void listSamples();
  bool loadCustomSample(int slot, const char* filename);
};
