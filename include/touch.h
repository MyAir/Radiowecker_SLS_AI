#ifndef TOUCH_H
#define TOUCH_H

#include <Wire.h>
#include "TAMC_GT911.h"

// Touch screen configuration
#define TOUCH_GT911

// Pins for ESP32-S3
#define TOUCH_GT911_SCL 18
#define TOUCH_GT911_SDA 17
#define TOUCH_GT911_INT -1  // Not using interrupt
#define TOUCH_GT911_RST 38  // Reset pin for GT911

// I2C address
#define TOUCH_GT911_ADDR 0x5D

// Display dimensions
#define TOUCH_GT911_WIDTH 800
#define TOUCH_GT911_HEIGHT 480

// Touch point structure for LVGL
struct TouchPoint {
    int16_t x;      // X coordinate
    int16_t y;      // Y coordinate
    bool pressed;   // Touch state
};

// Global touch controller instance
extern TAMC_GT911 gt911;
extern bool touch_initialized;

// Function declarations
void touch_init();
void touch_interrupt();
void touch_scan_i2c();
bool getTouchPoint(TouchPoint& point);

// Backward compatibility
typedef TouchPoint touch_point_t;

#endif // TOUCH_H
