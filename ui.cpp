/*
 * DriftRiff Mini - UI Module Implementation
 */

#include "ui.h"

UI::UI() {
  display = nullptr;
  lastCurrentStep = -1;
  lastBPM = -1;
  lastPlayState = false;
}

void UI::init(Adafruit_ILI9341* tft) {
  display = tft;
  display->setTextSize(1);
  display->setTextColor(COLOR_TEXT);
}

void UI::drawInterface() {
  display->fillScreen(COLOR_BG);
  
  // Title
  display->setCursor(10, 10);
  display->setTextSize(2);
  display->setTextColor(COLOR_STEP_ON);
  display->println("DRIFTRIFT MINI");
  
  // Track labels
  display->setTextSize(1);
  display->setTextColor(COLOR_TEXT);
  const char* trackNames[] = {"KICK", "SNARE", "HIHAT", "PERC", "BASS", "LEAD"};
  
  for (int track = 0; track < 6; track++) {
    int y = GRID_START_Y + (track * (STEP_HEIGHT + TRACK_SPACING)) + 2;
    display->setCursor(0, y);
    display->println(trackNames[track]);
  }
  
  // Draw grid outline
  int gridWidth = 16 * (STEP_WIDTH + STEP_SPACING) - STEP_SPACING;
  int gridHeight = 6 * (STEP_HEIGHT + TRACK_SPACING) - TRACK_SPACING;
  
  display->drawRect(GRID_START_X - 1, GRID_START_Y - 1, 
                   gridWidth + 2, gridHeight + 2, COLOR_GRID);
  
  // Step numbers
  display->setTextColor(COLOR_GRID);
  for (int step = 0; step < 16; step++) {
    int x = GRID_START_X + (step * (STEP_WIDTH + STEP_SPACING)) + 4;
    display->setCursor(x, GRID_START_Y - 15);
    if (step < 9) {
      display->print("0");
    }
    display->print(step + 1);
  }
  
  // Control buttons
  drawButton(BPM_X, CONTROL_Y, 80, 25, "BPM: 120");
  drawButton(PLAY_X, CONTROL_Y, 60, 25, "PLAY");
  drawButton(BPM_UP_X, CONTROL_Y, 30, 25, "+");
  drawButton(BPM_DOWN_X, CONTROL_Y, 30, 25, "-");
  
  // Initial grid draw
  for (int track = 0; track < 6; track++) {
    for (int step = 0; step < 16; step++) {
      drawStep(track, step, false, false);
    }
  }
}

void UI::updateGrid(bool steps[][16], int currentStep) {
  // Only redraw if current step changed
  if (currentStep != lastCurrentStep) {
    // Clear previous current step indicator
    if (lastCurrentStep >= 0) {
      for (int track = 0; track < 6; track++) {
        drawStep(track, lastCurrentStep, steps[track][lastCurrentStep], false);
      }
    }
    
    // Draw new current step
    for (int track = 0; track < 6; track++) {
      drawStep(track, currentStep, steps[track][currentStep], true);
    }
    
    lastCurrentStep = currentStep;
  }
  
  // Update any changed steps
  for (int track = 0; track < 6; track++) {
    for (int step = 0; step < 16; step++) {
      if (step != currentStep) {
        drawStep(track, step, steps[track][step], false);
      }
    }
  }
}

void UI::updateBPM(int bpm) {
  if (bpm != lastBPM) {
    // Clear previous BPM display
    display->fillRect(BPM_X + 1, CONTROL_Y + 1, 78, 23, COLOR_BG);
    
    // Draw new BPM
    display->setCursor(BPM_X + 8, CONTROL_Y + 10);
    display->setTextColor(COLOR_TEXT);
    display->print("BPM:");
    display->print(bpm);
    
    lastBPM = bpm;
  }
}

void UI::updatePlayState(bool isPlaying) {
  if (isPlaying != lastPlayState) {
    // Clear button area
    display->fillRect(PLAY_X + 1, CONTROL_Y + 1, 58, 23, COLOR_BG);
    
    // Draw play/pause state
    display->setCursor(PLAY_X + 15, CONTROL_Y + 10);
    display->setTextColor(COLOR_TEXT);
    display->print(isPlaying ? "PAUSE" : "PLAY");
    
    lastPlayState = isPlaying;
  }
}

void UI::drawStep(int track, int step, bool active, bool isCurrent) {
  int x = GRID_START_X + (step * (STEP_WIDTH + STEP_SPACING));
  int y = GRID_START_Y + (track * (STEP_HEIGHT + TRACK_SPACING));
  
  uint16_t color;
  if (isCurrent) {
    color = COLOR_CURRENT;
  } else if (active) {
    color = COLOR_STEP_ON;
  } else {
    color = COLOR_STEP_OFF;
  }
  
  display->fillRect(x, y, STEP_WIDTH, STEP_HEIGHT, color);
  
  // Draw border for current step
  if (isCurrent) {
    display->drawRect(x, y, STEP_WIDTH, STEP_HEIGHT, COLOR_STEP_ON);
  }
}

void UI::drawButton(int x, int y, int w, int h, const char* text, bool pressed) {
  uint16_t bgColor = pressed ? COLOR_STEP_ON : COLOR_BG;
  uint16_t textColor = pressed ? COLOR_BG : COLOR_TEXT;
  uint16_t borderColor = COLOR_GRID;
  
  display->fillRect(x, y, w, h, bgColor);
  display->drawRect(x, y, w, h, borderColor);
  
  // Center text
  int textWidth = strlen(text) * 6; // Approximate character width
  int textX = x + (w - textWidth) / 2;
  int textY = y + (h - 8) / 2;
  
  display->setCursor(textX, textY);
  display->setTextColor(textColor);
  display->print(text);
}

void UI::clearGrid() {
  int gridWidth = 16 * (STEP_WIDTH + STEP_SPACING) - STEP_SPACING;
  int gridHeight = 6 * (STEP_HEIGHT + TRACK_SPACING) - TRACK_SPACING;
  
  display->fillRect(GRID_START_X, GRID_START_Y, gridWidth, gridHeight, COLOR_BG);
}

bool UI::isInGridArea(int x, int y, int& track, int& step) {
  // Convert touch coordinates to grid position
  if (x < GRID_START_X || y < GRID_START_Y) return false;
  
  int relX = x - GRID_START_X;
  int relY = y - GRID_START_Y;
  
  step = relX / (STEP_WIDTH + STEP_SPACING);
  track = relY / (STEP_HEIGHT + TRACK_SPACING);
  
  if (step >= 16 || track >= 6) return false;
  
  // Check if within actual step bounds
  int stepX = step * (STEP_WIDTH + STEP_SPACING);
  int stepY = track * (STEP_HEIGHT + TRACK_SPACING);
  
  return (relX >= stepX && relX < stepX + STEP_WIDTH &&
          relY >= stepY && relY < stepY + STEP_HEIGHT);
}

bool UI::isInBPMUpArea(int x, int y) {
  return (x >= BPM_UP_X && x <= BPM_UP_X + 30 &&
          y >= CONTROL_Y && y <= CONTROL_Y + 25);
}

bool UI::isInBPMDownArea(int x, int y) {
  return (x >= BPM_DOWN_X && x <= BPM_DOWN_X + 30 &&
          y >= CONTROL_Y && y <= CONTROL_Y + 25);
}

bool UI::isInPlayArea(int x, int y) {
  return (x >= PLAY_X && x <= PLAY_X + 60 &&
          y >= CONTROL_Y && y <= CONTROL_Y + 25);
}
