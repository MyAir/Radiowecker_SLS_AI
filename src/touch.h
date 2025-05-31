#ifndef TOUCH_H
#define TOUCH_H

#include <Wire.h>
#include "TAMC_GT911.h"

// Debug macro for touch
#ifndef TOUCH_DEBUG
  #define TOUCH_DEBUG 1
#endif

// Touch screen configuration
#define TOUCH_GT911

// Pins for ESP32-S3
#define TOUCH_GT911_SCL 18
#define TOUCH_GT911_SDA 17
#define TOUCH_GT911_INT -1  // Not using interrupt
#define TOUCH_GT911_RST 38  // Reset pin for GT911

// I2C address (will be detected)
extern uint8_t TOUCH_GT911_ADDR;
#define TOUCH_GT911_DEFAULT_ADDR 0x14  // Default I2C address for GT911

// Display dimensions
#define TOUCH_GT911_WIDTH 800
#define TOUCH_GT911_HEIGHT 480
#define TOUCH_GT911_ROTATION ROTATION_NORMAL

// Debug output macros
#if TOUCH_DEBUG
  #define TOUCH_LOG(fmt, ...) if(Serial) { Serial.print("[TOUCH] "); Serial.printf(fmt, ##__VA_ARGS__); }
  #define TOUCH_LOG_LN(fmt, ...) if(Serial) { Serial.print("[TOUCH] "); Serial.printf(fmt, ##__VA_ARGS__); Serial.println(); }
#else
  #define TOUCH_LOG(fmt, ...)
  #define TOUCH_LOG_LN(fmt, ...)
#endif

// Maximum number of touch points to track
#define MAX_TOUCH_POINTS 5

// Touch point structure for LVGL
typedef struct {
    int16_t x;     // X coordinate
    int16_t y;     // Y coordinate
    bool touched;  // Touch state
} TouchPoint;

// Global touch controller instance
extern TAMC_GT911 gt911;

extern TouchPoint touchPoints[];
extern bool touch_initialized;

// Function declarations
void touch_init();
void TouchonInterrupt();
void resetGT911();
void scanI2C(TwoWire &wire);
bool getTouchPoint(TouchPoint* point);

#endif // TOUCH_H

