#include "touch.h"
#include <Arduino.h>

// Global touch object
TAMC_GT911 ts = TAMC_GT911(TOUCH_GT911_SDA, TOUCH_GT911_SCL, TOUCH_GT911_INT, TOUCH_GT911_RST, 800, 480);

// Last touch point for debouncing
static TouchPoint lastTouchPoint = {0};
static uint32_t lastTouchTime = 0;

void touch_init() {
    // Initialize reset pin
    pinMode(TOUCH_GT911_RST, OUTPUT);
    
    // Reset sequence for GT911
    digitalWrite(TOUCH_GT911_RST, LOW);
    delay(10);
    digitalWrite(TOUCH_GT911_RST, HIGH);
    delay(10);
    
    // Initialize I2C
    Wire.begin(TOUCH_GT911_SDA, TOUCH_GT911_SCL);
    
    // Initialize touch controller
    ts.begin();
    if (Serial) {
        if (ts.isTouched) {
            Serial.println("Touch controller initialized successfully");
        } else {
            Serial.println("Warning: Touch controller may not be responding correctly");
        }
    }
    
    // Set rotation
    ts.setRotation(TOUCH_GT911_ROTATION);
    
    if (Serial) {
        Serial.println("Touch controller ready");
    }
}

void TouchonInterrupt() {
    // This function is called when a touch interrupt occurs
    ts.isTouched = true;
}

bool getTouchPoint(TouchPoint* point) {
    if (!point) return false;
    
    // Read touch data
    ts.read();
    
    // Check if touch is active and coordinates are valid
    if (ts.isTouched && ts.points[0].x > 0 && ts.points[0].y > 0) {
        // Update last touch point
        lastTouchPoint.x = ts.points[0].x;
        lastTouchPoint.y = ts.points[0].y;
        lastTouchPoint.touched = true;
        lastTouchTime = millis();
        
        // Copy to output
        *point = lastTouchPoint;
        return true;
    } else if (lastTouchPoint.touched && (millis() - lastTouchTime < 50)) {
        // Touch release debounce (50ms)
        *point = lastTouchPoint;
        return true;
    } else {
        // No touch
        lastTouchPoint.touched = false;
        return false;
    }
}
