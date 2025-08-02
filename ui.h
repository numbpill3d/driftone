/*
 * DriftRiff Mini - UI Module Header
 */

#ifndef UI_H
#define UI_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// Colors (minimalist black/red theme)
#define COLOR_BG        ILI9341_BLACK
#define COLOR_GRID      0x2104        // Dark gray
#define COLOR_STEP_OFF  0x4208        // Medium gray
#define COLOR_STEP_ON   ILI9341_RED
#define COLOR_CURRENT   ILI9341_WHITE
#define COLOR_TEXT      ILI9341_WHITE

// Layout constants
#define GRID_START_X    20
#define GRID_START_Y    40
#define STEP_WIDTH      16
#define STEP_HEIGHT     12
#define STEP_SPACING    2
#define TRACK_SPACING   3

#define CONTROL_Y       200
#define BPM_X           20
#define PLAY_X          120
#define BPM_UP_X        220
#define BPM_DOWN_X      260

class UI {
private:
  Adafruit_ILI9341* display;
  int lastCurrentStep;
  int lastBPM;
  bool lastPlayState;
  
public:
  UI();
  
  void init(Adafruit_ILI9341* tft);
  void drawInterface();
  void updateGrid(bool steps[][16], int currentStep);
  void updateBPM(int bpm);
  void updatePlayState(bool isPlaying);
  
  // Helper functions
  void drawStep(int track, int step, bool active, bool isCurrent);
  void drawButton(int x, int y, int w, int h, const char* text, bool pressed = false);
  void clearGrid();
  
  // Touch area helpers
  bool isInGridArea(int x, int y, int& track, int& step);
  bool isInBPMUpArea(int x, int y);
  bool isInBPMDownArea(int x, int y);
  bool isInPlayArea(int x, int y);
};
