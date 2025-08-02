/*
 * DriftRiff Mini - Touchscreen Handler Implementation
 */

#include "touchscreen.h"
#include "ui.h"

TouchHandler::TouchHandler() {
  touchScreen = nullptr;
  display = nullptr;
  lastTouchTime = 0;
  lastTouchX = -1;
  lastTouchY = -1;
}

void TouchHandler::init(TouchScreen* ts, Adafruit_ILI9341* tft) {
  touchScreen = ts;
  display = tft;
  Serial.println("Touch handler initialized");
}

TSPoint TouchHandler::getTouch() {
  if (!touchScreen) {
    return TSPoint(0, 0, 0);
  }
  
  TSPoint p = touchScreen->getPoint();
  
  // Restore pins for TFT use
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  
  return p;
}

TouchAction TouchHandler::processTouchInput(int rawX, int rawY) {
  TouchAction action;
  action.type = TOUCH_NONE;
  action.track = -1;
  action.step = -1;
  
  // Map raw coordinates to screen coordinates
  int screenX = mapTouchX(rawX);
  int screenY = mapTouchY(rawY);
  
  action.x = screenX;
  action.y = screenY;
  
  Serial.print("Touch: Raw(");
  Serial.print(rawX);
  Serial.print(",");
  Serial.print(rawY);
  Serial.print(") -> Screen(");
  Serial.print(screenX);
  Serial.print(",");
  Serial.print(screenY);
  Serial.println(")");
  
  // Check if touch is in grid area
  int track, step;
  if (screenX >= GRID_START_X && screenY >= GRID_START_Y) {
    // Calculate grid position
    int relX = screenX - GRID_START_X;
    int relY = screenY - GRID_START_Y;
    
    step = relX / (STEP_WIDTH + STEP_SPACING);
    track = relY / (STEP_HEIGHT + TRACK_SPACING);
    
    // Validate grid bounds and check if within step bounds
    if (step >= 0 && step < 16 && track >= 0 && track < 6) {
      int stepX = step * (STEP_WIDTH + STEP_SPACING);
      int stepY = track * (STEP_HEIGHT + TRACK_SPACING);
      
      if (relX >= stepX && relX < stepX + STEP_WIDTH &&
          relY >= stepY && relY < stepY + STEP_HEIGHT) {
        action.type = TOUCH_GRID;
        action.track = track;
        action.step = step;
        
        Serial.print("Grid touch - Track: ");
        Serial.print(track);
        Serial.print(", Step: ");
        Serial.println(step);
        
        return action;
      }
    }
  }
  
  // Check control buttons
  if (screenY >= CONTROL_Y && screenY <= CONTROL_Y + 25) {
    if (screenX >= BPM_UP_X && screenX <= BPM_UP_X + 30) {
      action.type = TOUCH_BPM_UP;
      Serial.println("BPM Up touched");
    }
    else if (screenX >= BPM_DOWN_X && screenX <= BPM_DOWN_X + 30) {
      action.type = TOUCH_BPM_DOWN;
      Serial.println("BPM Down touched");
    }
    else if (screenX >= PLAY_X && screenX <= PLAY_X + 60) {
      action.type = TOUCH_PLAY_PAUSE;
      Serial.println("Play/Pause touched");
    }
  }
  
  return action;
}

int TouchHandler::mapTouchX(int rawX) {
  // Map raw touch X to screen X coordinate
  return map(rawX, TS_MINX, TS_MAXX, 0, 320);
}

int TouchHandler::mapTouchY(int rawY) {
  // Map raw touch Y to screen Y coordinate  
  return map(rawY, TS_MINY, TS_MAXY, 0, 240);
}

void TouchHandler::calibrate() {
  if (!display) return;
  
  display->fillScreen(ILI9341_BLACK);
  display->setTextColor(ILI9341_WHITE);
  display->setTextSize(2);
  display->setCursor(50, 100);
  display->println("CALIBRATION");
  display->setTextSize(1);
  display->setCursor(30, 130);
  display->println("Touch corners and center");
  display->setCursor(30, 150);
  display->println("when prompted...");
  
  // Note: Full calibration implementation would require
  // collecting touch points and calculating transformation matrix
  // For now, using fixed calibration values
  
  delay(2000);
  Serial.println("Touch calibration complete (using default values)");
}

bool TouchHandler::isValidTouch(TSPoint p) {
  // Check if touch point is within reasonable bounds
  return (p.z >= 10 && p.z <= 1000 &&
          p.x >= TS_MINX && p.x <= TS_MAXX &&
          p.y >= TS_MINY && p.y <= TS_MAXY);
}
