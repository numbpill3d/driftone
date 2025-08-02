/*
 * DriftRiff Mini - SD Card Sample Loader Implementation
 */

#include "sdloader.h"

SDLoader::SDLoader() {
  for (int i = 0; i < NUM_SAMPLE_SLOTS; i++) {
    sampleData[i] = nullptr;
    sampleSizes[i] = 0;
    samplesLoaded[i] = false;
  }
}

SDLoader::~SDLoader() {
  unloadAllSamples();
}

void SDLoader::init() {
  Serial.println("SD Loader initialized");
}

bool SDLoader::loadAllSamples() {
  bool allLoaded = true;
  
  Serial.println("Loading samples...");
  
  for (int slot = 0; slot < NUM_SAMPLE_SLOTS; slot++) {
    if (!loadSample(slot, sampleFiles[slot])) {
      allLoaded = false;
      Serial.print("Failed to load sample for slot ");
      Serial.print(slot);
      Serial.print(": ");
      Serial.println(sampleFiles[slot]);
    }
  }
  
  if (allLoaded) {
    Serial.println("All samples loaded successfully");
  } else {
    Serial.println("Some samples failed to load - check SD card content");
  }
  
  return allLoaded;
}

bool SDLoader::loadSample(int slot, const char* filename) {
  if (slot < 0 || slot >= NUM_SAMPLE_SLOTS) {
    return false;
  }
  
  // Free existing sample if loaded
  freeSample(slot);
  
  // Check if file exists
  if (!SD.exists(filename)) {
    Serial.print("File not found: ");
    Serial.println(filename);
    return false;
  }
  
  File file = SD.open(filename, FILE_READ);
  if (!file) {
    Serial.print("Failed to open file: ");
    Serial.println(filename);
    return false;
  }
  
  uint32_t fileSize = file.size();
  if (fileSize == 0) {
    Serial.print("Empty file: ");
    Serial.println(filename);
    file.close();
    return false;
  }
  
  if (fileSize > MAX_SAMPLE_SIZE) {
    Serial.print("File too large (");
    Serial.print(fileSize);
    Serial.print(" bytes): ");
    Serial.println(filename);
    fileSize = MAX_SAMPLE_SIZE;
  }
  
  // Allocate memory for sample
  sampleData[slot] = (uint8_t*)malloc(fileSize);
  if (!sampleData[slot]) {
    Serial.print("Failed to allocate memory for: ");
    Serial.println(filename);
    file.close();
    return false;
  }
  
  // Read file data
  uint32_t bytesRead = file.read(sampleData[slot], fileSize);
  file.close();
  
  if (bytesRead != fileSize) {
    Serial.print("Read error for file: ");
    Serial.println(filename);
    freeSample(slot);
    return false;
  }
  
  sampleSizes[slot] = fileSize;
  samplesLoaded[slot] = true;
  
  Serial.print("Loaded sample ");
  Serial.print(slot);
  Serial.print(" (");
  Serial.print(fileSize);
  Serial.print(" bytes): ");
  Serial.println(filename);
  
  return true;
}

void SDLoader::freeSample(int slot) {
  if (slot < 0 || slot >= NUM_SAMPLE_SLOTS) {
    return;
  }
  
  if (sampleData[slot]) {
    free(sampleData[slot]);
    sampleData[slot] = nullptr;
  }
  
  sampleSizes[slot] = 0;
  samplesLoaded[slot] = false;
}

void SDLoader::unloadAllSamples() {
  for (int slot = 0; slot < NUM_SAMPLE_SLOTS; slot++) {
    freeSample(slot);
  }
  Serial.println("All samples unloaded");
}

uint8_t* SDLoader::getSampleData(int slot) {
  if (slot < 0 || slot >= NUM_SAMPLE_SLOTS || !samplesLoaded[slot]) {
    return nullptr;
  }
  return sampleData[slot];
}

uint32_t SDLoader::getSampleSize(int slot) {
  if (slot < 0 || slot >= NUM_SAMPLE_SLOTS || !samplesLoaded[slot]) {
    return 0;
  }
  return sampleSizes[slot];
}

bool SDLoader::isSampleLoaded(int slot) {
  if (slot < 0 || slot >= NUM_SAMPLE_SLOTS) {
    return false;
  }
  return samplesLoaded[slot];
}

void SDLoader::listSamples() {
  Serial.println("Sample Status:");
  const char* trackNames[] = {"KICK", "SNARE", "HIHAT", "PERC", "BASS", "LEAD"};
  
  for (int slot = 0; slot < NUM_SAMPLE_SLOTS; slot++) {
    Serial.print(trackNames[slot]);
    Serial.print(": ");
    
    if (samplesLoaded[slot]) {
      Serial.print("LOADED (");
      Serial.print(sampleSizes[slot]);
      Serial.print(" bytes) - ");
      Serial.println(sampleFiles[slot]);
    } else {
      Serial.print("NOT LOADED - ");
      Serial.println(sampleFiles[slot]);
    }
  }
}

bool SDLoader::loadCustomSample(int slot, const char* filename) {
  if (slot < 0 || slot >= NUM_SAMPLE_SLOTS) {
    return false;
  }
  
  return loadSample(slot, filename);
}
