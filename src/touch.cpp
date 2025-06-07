#include "touch.h"
#include <Arduino.h>
#include <Wire.h>

// Debug configuration
#ifndef TOUCH_DEBUG
#define TOUCH_DEBUG 0  // Set to 1 to enable debug output
#endif

#if TOUCH_DEBUG
#define TOUCH_LOG(msg) Serial.print(msg)
#define TOUCH_LOG_LN(msg) Serial.println(msg)
#else
#define TOUCH_LOG(msg)
#define TOUCH_LOG_LN(msg)
#endif

// Global touch controller instance
TAMC_GT911 gt911(TOUCH_GT911_SDA, TOUCH_GT911_SCL, TOUCH_GT911_INT, TOUCH_GT911_RST, 
                TOUCH_GT911_WIDTH, TOUCH_GT911_HEIGHT);
bool touch_initialized = false;

// Initialize the touch controller
void touch_init() {
    TOUCH_LOG_LN("Initializing GT911 touch controller...");
    
    // Initialize I2C with 400kHz for faster response
    // Wire.begin(TOUCH_GT911_SDA, TOUCH_GT911_SCL);
    // Wire.setClock(400000);
    // TOUCH_LOG_LN("I2C initialized");
    
    // Reset the touch controller if reset pin is available
    if (TOUCH_GT911_RST >= 0) {
        TOUCH_LOG_LN("Resetting touch controller");
        pinMode(TOUCH_GT911_RST, OUTPUT);
        digitalWrite(TOUCH_GT911_RST, LOW);
        delay(500);
        digitalWrite(TOUCH_GT911_RST, HIGH);
        delay(500);
    }
    
    // Initialize the GT911 with the specified I2C address
    TOUCH_LOG_LN("Calling gt911.begin()...");
    gt911.begin(TOUCH_GT911_ADDR);
    
    // Assume initialization succeeded
    TOUCH_LOG_LN("GT911 initialized");
    
    // Set rotation (0-3, where 3 is 270° rotation if needed)
    gt911.setRotation(ROTATION_NORMAL);
    TOUCH_LOG_LN("Touch rotation set to normal");
    
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
        
        #if TOUCH_DEBUG
        Serial.print("Touch at (");
        Serial.print(point.x);
        Serial.print(", ");
        Serial.print(point.y);
        Serial.println(")");
        #endif
        
        return true;
    }
    
    // No touch detected
    point.x = 0;
    point.y = 0;
    point.pressed = false;
    return false;
}

// Empty interrupt handler (not used)
void touch_interrupt() {
    // Not used in current implementation
    // Can be implemented if interrupt pin is connected
}

// I2C scanner for debug
void touch_scan_i2c() {
    #if TOUCH_DEBUG
    byte error, address;
    int deviceCount = 0;
    
    Serial.println("Scanning I2C bus...");
    
    for (address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        
        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16) {
                Serial.print("0");
            }
            Serial.print(address, HEX);
            Serial.println();
            deviceCount++;
        }
    }
    
    if (deviceCount == 0) {
        Serial.println("No I2C devices found");
    } else {
        Serial.print("Found ");
        Serial.print(deviceCount);
        Serial.println(" device(s)");
    }
    #endif
}
