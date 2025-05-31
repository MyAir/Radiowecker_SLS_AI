#include "touch.h"
#include <Arduino.h>
#include <Wire.h>

// Global touch controller instance
TAMC_GT911 gt911(TOUCH_GT911_SDA, TOUCH_GT911_SCL, TOUCH_GT911_INT, TOUCH_GT911_RST, 
                TOUCH_GT911_WIDTH, TOUCH_GT911_HEIGHT);
bool touch_initialized = false;

// Reset the GT911 touch controller
void resetGT911() {
    if (TOUCH_GT911_RST >= 0) {
        pinMode(TOUCH_GT911_RST, OUTPUT);
        digitalWrite(TOUCH_GT911_RST, LOW);
        delay(10);
        digitalWrite(TOUCH_GT911_RST, HIGH);
        delay(50);
    }
}

// Initialize the touch controller
void touch_init() {
    // Initialize I2C with 400kHz for faster response
    Wire.begin(TOUCH_GT911_SDA, TOUCH_GT911_SCL);
    Wire.setClock(400000);
    
    // Reset the touch controller
    resetGT911();
    
    // Initialize the GT911 with the specified I2C address
    gt911.begin(TOUCH_GT911_ADDR);
    
    // Set rotation (0-3, where 3 is 270° rotation if needed)
    gt911.setRotation(3);
    
    touch_initialized = true;
}

// Read the touch position
bool getTouchPoint(TouchPoint& point) {
    if (!touch_initialized) {
        point.pressed = false;
        return false;
    }
    
    // Read touch data from the controller
    gt911.read();
    
    // Check if there are any touches
    if (gt911.isTouched) {
        // Return the first touch point with coordinates flipped
        point.x = TOUCH_GT911_WIDTH - gt911.points[0].x - 1;  // Flip X axis
        point.y = TOUCH_GT911_HEIGHT - gt911.points[0].y - 1; // Flip Y axis
        point.pressed = true;
        return true;
    }
    
    // No touch detected
    point.x = 0;
    point.y = 0;
    point.pressed = false;
    return false;
}

// Empty interrupt handler (not used)
void touch_interrupt() {}

// Empty I2C scanner (not used)
void touch_scan_i2c() {}
