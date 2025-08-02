/*
 * DriftRiff Mini - Touchscreen Handler Header
 */

#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include <TouchScreen.h>
#include <Adafruit_ILI9341.h>

// Touch pins - Updated for ESP32 GPIO capabilities
#define YP 34  // Y+ must be analog capable (input-only on ESP32)
#define XM 33  // X- must be output capable
#define YM 14  // Y- can be any digital pin
#define XP 32  // X+ must be analog capable

// Touch calibration values (adjust based on your screen)
#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

// Touch action types
enum TouchActionType {
  TOUCH_NONE,
  TOUCH_GRID,
  TOUCH_BPM_UP,
  TOUCH_BPM_DOWN,
  TOUCH_PLAY_PAUSE,
  TOUCH_CLEAR_TRACK,
  TOUCH_CLEAR_ALL
};

struct TouchAction {
  TouchActionType type;
  int track;
  int step;
  int x;
  int y;
};

class TouchHandler {
private:
  TouchScreen* touchScreen;
  Adafruit_ILI9341* display;
  
  unsigned long lastTouchTime;
  int lastTouchX, lastTouchY;
  
  // Coordinate transformation
  int mapTouchX(int rawX);
  int mapTouchY(int rawY);
  
public:
  TouchHandler();
  
  void init(TouchScreen* ts, Adafruit_ILI9341* tft);
  TSPoint getTouch();
  TouchAction processTouchInput(int rawX, int rawY);
  
  void calibrate();
  bool isValidTouch(TSPoint p);
};
