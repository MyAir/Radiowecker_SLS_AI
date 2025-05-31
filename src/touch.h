#ifndef TOUCH_H
#define TOUCH_H

#include <Wire.h>
#include "TAMC_GT911.h"

// Touch screen configuration
#define TOUCH_GT911
#define TOUCH_GT911_SCL 18
#define TOUCH_GT911_SDA 17
#define TOUCH_GT911_INT -1  // No interrupt pin, use polling
#define TOUCH_GT911_RST 38  // Reset pin
#define TOUCH_GT911_ROTATION ROTATION_NORMAL

// Global touch object
extern TAMC_GT911 ts;

// Function declarations
void touch_init();
void TouchonInterrupt();

// Touch point structure for LVGL
typedef struct {
    int16_t x;
    int16_t y;
    bool touched;
} TouchPoint;

// Function to get the last touch point
bool getTouchPoint(TouchPoint* point);

#endif // TOUCH_H

