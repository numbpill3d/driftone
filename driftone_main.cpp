/*
 * Driftone Mini - ESP32 Beat Sequencer
 * Main Application File
 */

#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <TouchScreen.h>

#include "sequencer.h"
#include "ui.h"
#include "audioengine.h"
#include "sdloader.h"
#include "touchscreen.h"

// Pin definitions for ILI9341
#define TFT_CS     5
#define TFT_RST    4
#define TFT_DC     2
#define TFT_MOSI   23
#define TFT_CLK    18
#define TFT_MISO   19

// SD card pins
#define SD_CS      15

// Touch pins (4-wire resistive)
#define YP A2  // must be an analog pin
#define XM A3  // must be an analog pin  
#define YM 14  // can be a digital pin
#define XP 27  // can be a digital pin

// Touch pressure range
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// Global objects
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

Sequencer sequencer;
UI ui;
AudioEngine audioEngine;
SDLoader sdLoader;
TouchHandler touchHandler;

// Timing variables
unsigned long lastStepTime = 0;
unsigned long stepInterval = 125; // 120 BPM default (500ms / 4 = 125ms per 16th note)

void setup() {
  Serial.begin(115200);
  Serial.println("DriftRiff Mini Starting...");
  
  // Initialize TFT
  tft.begin();
  tft.setRotation(3); // Landscape
  tft.fillScreen(ILI9341_BLACK);
  
  // Initialize SD card
  if (!SD.begin(SD_CS)) {
    Serial.println("SD Card initialization failed!");
    tft.setCursor(10, 10);
    tft.setTextColor(ILI9341_RED);
    tft.println("SD CARD ERROR");
    while (1);
  }
  
  // Initialize modules
  sequencer.init();
  ui.init(&tft);
  audioEngine.init();
  sdLoader.init();
  touchHandler.init(&ts, &tft);
  
  // Load samples
  if (!sdLoader.loadAllSamples()) {
    Serial.println("Warning: Some samples failed to load");
  }
  
  // Initial UI draw
  ui.drawInterface();
  ui.updateGrid(sequencer.getSteps(), sequencer.getCurrentStep());
  
  Serial.println("DriftRiff Mini Ready!");
}

void loop() {
  unsigned long currentTime = millis();
  
  // Handle sequencer timing
  if (currentTime - lastStepTime >= stepInterval) {
    lastStepTime = currentTime;
    
    // Advance step
    sequencer.nextStep();
    
    // Check for active steps and trigger samples
    for (int track = 0; track < 6; track++) {
      if (sequencer.isStepActive(track, sequencer.getCurrentStep())) {
        uint8_t* sampleData = sdLoader.getSampleData(track);
        uint32_t sampleSize = sdLoader.getSampleSize(track);
        
        if (sampleData && sampleSize > 0) {
          audioEngine.playSample(sampleData, sampleSize);
        }
      }
    }
    
    // Update UI
    ui.updateGrid(sequencer.getSteps(), sequencer.getCurrentStep());
    ui.updateBPM(sequencer.getBPM());
  }
  
  // Handle touch input
  TSPoint p = touchHandler.getTouch();
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    TouchAction action = touchHandler.processTouchInput(p.x, p.y);
    
    switch (action.type) {
      case TOUCH_GRID:
        sequencer.toggleStep(action.track, action.step);
        ui.updateGrid(sequencer.getSteps(), sequencer.getCurrentStep());
        break;
        
      case TOUCH_BPM_UP:
        sequencer.increaseBPM();
        stepInterval = 60000 / (sequencer.getBPM() * 4); // Update timing
        ui.updateBPM(sequencer.getBPM());
        break;
        
      case TOUCH_BPM_DOWN:
        sequencer.decreaseBPM();
        stepInterval = 60000 / (sequencer.getBPM() * 4); // Update timing
        ui.updateBPM(sequencer.getBPM());
        break;
        
      case TOUCH_PLAY_PAUSE:
        sequencer.togglePlayback();
        ui.updatePlayState(sequencer.isPlaying());
        break;
        
      default:
        break;
    }
    
    delay(50); // Simple debounce
  }
  
  // Audio engine update (handles PWM timing)
  audioEngine.update();
}
