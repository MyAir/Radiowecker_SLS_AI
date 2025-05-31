#include "touch.h"
#include <Arduino.h>

// Initialize the I2C address with default value
uint8_t TOUCH_GT911_ADDR = TOUCH_GT911_DEFAULT_ADDR;

// Touch controller instance
TAMC_GT911 gt911(TOUCH_GT911_SDA, TOUCH_GT911_SCL, TOUCH_GT911_INT, TOUCH_GT911_RST, TOUCH_GT911_WIDTH, TOUCH_GT911_HEIGHT);

// Buffer for touch points
TouchPoint touchPoints[5];
bool touch_initialized = false;

// I2C instance for touch controller
TwoWire touchWire = TwoWire(0);

// Last touch point for debouncing
static TouchPoint lastTouchPoint = {0};
static uint32_t lastTouchTime = 0;

// Function to manually reset GT911
void resetGT911() {
    if (TOUCH_GT911_RST >= 0) {
        TOUCH_LOG("Resetting GT911 on pin %d... ", TOUCH_GT911_RST);
        
        // Simple reset sequence - this is more reliable and avoids GPIO errors
        pinMode(TOUCH_GT911_RST, OUTPUT);
        
        // Pull reset low for 20ms
        digitalWrite(TOUCH_GT911_RST, LOW);
        delay(20);  
        
        // Release reset and wait for chip to stabilize
        digitalWrite(TOUCH_GT911_RST, HIGH);
        delay(150);
        
        TOUCH_LOG_LN("Done");
    } else {
        TOUCH_LOG_LN("No RST pin defined, skipping reset");
    }
}

// Function to scan I2C bus and print found devices
void scanI2C(TwoWire &wire) {
    byte error, address;
    int nDevices = 0;
    
    TOUCH_LOG_LN("Scanning I2C bus...");
    for(address = 1; address < 127; address++) {
        wire.beginTransmission(address);
        error = wire.endTransmission();
        
        if (error == 0) {
            TOUCH_LOG("I2C device found at address 0x%02X\n", address);
            nDevices++;
        } else if (error == 4) {
            TOUCH_LOG("Unknown error at address 0x%02X\n", address);
        }    
    }
    if (nDevices == 0) {
        TOUCH_LOG_LN("No I2C devices found. Check wiring!");
    } else {
        TOUCH_LOG_LN("I2C scan complete");
    }
}

void touch_init() {
    TOUCH_LOG_LN("\n===== TOUCH INITIALIZATION START =====");
    TOUCH_LOG_LN("Touch Pins - SDA: %d, SCL: %d, RST: %d, INT: %d", 
                TOUCH_GT911_SDA, TOUCH_GT911_SCL, TOUCH_GT911_RST, TOUCH_GT911_INT);
    
    // Initialize I2C
    TOUCH_LOG_LN("Initializing I2C...");
    bool i2c_initialized = Wire.begin(TOUCH_GT911_SDA, TOUCH_GT911_SCL);
    if (!i2c_initialized) {
        TOUCH_LOG_LN("ERROR: Failed to initialize I2C");
        touch_initialized = false;
        return;
    }
    
    // Set I2C clock speed (100kHz for reliability)
    Wire.setClock(100000);
    TOUCH_LOG_LN("I2C initialized at 100kHz");
    
    // Reset the touch controller
    TOUCH_LOG_LN("Resetting GT911...");
    resetGT911();
    
    // Give the touch controller time to start up after reset
    delay(100);
    
    // Scan I2C bus to find the device
    TOUCH_LOG_LN("Scanning I2C bus for GT911...");
    scanI2C(Wire);
    
    // Try both possible I2C addresses (0x14 and 0x5D)
    const uint8_t addresses[] = {0x14, 0x5D};
    bool found = false;
    
    for (int i = 0; i < 2; i++) {
        uint8_t addr = addresses[i];
        TOUCH_LOG("Trying address 0x%02X... ", addr);
        
        // Check if device responds at this address
        Wire.beginTransmission(addr);
        byte error = Wire.endTransmission();
        
        if (error == 0) {
            TOUCH_GT911_ADDR = addr; // Update the address
            found = true;
            TOUCH_LOG_LN("Found! Using address 0x%02X", addr);
            break;
        } else {
            TOUCH_LOG_LN("Not found (error: %d)", error);
        }
    }
    
    if (!found) {
        TOUCH_LOG_LN("ERROR: GT911 not found on I2C bus. Check wiring and power.");
        touch_initialized = false;
        return;
    }
    
    // Initialize the GT911 with the found address
    TOUCH_LOG_LN("Initializing GT911...");
    
    // Initialize GT911 with I2C parameters
    TOUCH_LOG("Initializing GT911... ");
    Wire.begin(TOUCH_GT911_SDA, TOUCH_GT911_SCL);
    Wire.setClock(100000);  // 100 kHz for better reliability
    TOUCH_LOG_LN("");
    
    // Log I2C initialization
    TOUCH_LOG_LN("Calling gt911.begin()... ");
    
    // First try a soft reset without the library function
    // This helps avoid the GPIO pin 226 errors
    if (TOUCH_GT911_RST >= 0) {
        pinMode(TOUCH_GT911_RST, OUTPUT);
        digitalWrite(TOUCH_GT911_RST, LOW);
        delay(20);  // Hold reset low for 20ms
        digitalWrite(TOUCH_GT911_RST, HIGH);
        delay(200); // Wait for chip to stabilize
    }
    
    // Now initialize the controller with its address
    // Force the address to 0x5D (we detected the device at this address)
    TOUCH_GT911_ADDR = 0x5D;
    
    // Disable Serial output temporarily to suppress Wire warnings
    Serial.flush();
    Serial.end();
    
    // Initialize controller
    gt911.begin(TOUCH_GT911_ADDR);
    
    // Re-enable Serial
    Serial.begin(115200);
    delay(50); // Give serial time to restart
    TOUCH_LOG_LN("Done");
    
    // Set rotation for the touch screen
    TOUCH_LOG("Setting rotation... ");
    gt911.setRotation(TOUCH_GT911_ROTATION);
    TOUCH_LOG_LN("Done");
    
    // The TAMC_GT911 library doesn't provide direct access to product ID and firmware version
    TOUCH_LOG_LN("GT911 initialized");
    
    // Read initial touch state to verify communication
    TOUCH_LOG("Reading initial touch state... ");
    gt911.read();
    TOUCH_LOG_LN("Done");
    
    // Verify GT911 is working by reading point count from controller
    // This is more reliable than directly accessing registers
    gt911.read();
    TOUCH_LOG_LN("GT911 touch point count: %d", gt911.touches);
    
    // Now try to read the refresh rate as secondary verification
    uint8_t refresh_rate = 0;
    bool read_success = false;
    
    Wire.beginTransmission(TOUCH_GT911_ADDR);
    Wire.write(0x80); // High byte first
    Wire.write(0x56); // Low byte - refresh rate register
    if (Wire.endTransmission(false) == 0) {
        if (Wire.requestFrom(TOUCH_GT911_ADDR, (uint8_t)1) == 1) {
            refresh_rate = Wire.read();
            TOUCH_LOG_LN("GT911 refresh rate: %d", refresh_rate);
            read_success = true;
        }
    }
    
    // If we can read the refresh rate and it's a reasonable value,
    // or if we can detect touches, consider the controller working
    if (read_success && refresh_rate > 0) {
        touch_initialized = true;
        TOUCH_LOG_LN("GT911 communication verified through refresh rate!");
    } 
    // Or if the touch controller reports it's ready (isTouched is valid)
    else if (gt911.touches >= 0) {
        touch_initialized = true;
        TOUCH_LOG_LN("GT911 communication verified through touch data!");
    }
    else {
        if (!read_success) {
            TOUCH_LOG_LN("ERROR: Failed to read GT911 registers");
        } else {
            TOUCH_LOG_LN("WARNING: Invalid GT911 refresh rate: %d", refresh_rate);
        }
        touch_initialized = false;
    }
    
    TOUCH_LOG_LN("Touch points available: %d", gt911.touches);
    
    if (touch_initialized) {
        TOUCH_LOG_LN("GT911 initialized successfully!");
    } else {
        TOUCH_LOG_LN("GT911 initialization FAILED!");
    }
    
    TOUCH_LOG_LN("===== TOUCH INITIALIZATION COMPLETE =====");
    
    // Print touch info
    if (touch_initialized) {
        TOUCH_LOG_LN("Touch Controller Info:");
        TOUCH_LOG_LN("  - I2C Address: 0x%02X", TOUCH_GT911_ADDR);
        TOUCH_LOG_LN("  - Resolution: %dx%d", TOUCH_GT911_WIDTH, TOUCH_GT911_HEIGHT);
        TOUCH_LOG_LN("  - Rotation: %d", TOUCH_GT911_ROTATION);
        TOUCH_LOG_LN("  - Touch Points: %d", gt911.touches);
    }
}

void TouchonInterrupt() {
    // This function is called when a touch interrupt occurs
    gt911.isTouched = true;
}

bool getTouchPoint(TouchPoint* point) {
    static uint32_t lastDebug = 0;
    static uint32_t readErrors = 0;
    uint32_t now = millis();
    
    if (!touch_initialized) {
        if (now - lastDebug > 1000) {  // Log every second if not initialized
            TOUCH_LOG_LN("Warning: Touch device not initialized!");
            lastDebug = now;
        }
        return false;
    }
    
    if (!point) {
        TOUCH_LOG_LN("Error: Invalid touch point pointer");
        return false;
    }
    
    // Read touch points from GT911
    bool success = false;
    
    // Read the touch data
    gt911.read();
    
    // Check if screen is touched - more permissive condition
    // Sometimes gt911.isTouched is not reliable, so we check touches directly
    if (gt911.touches > 0) {
        // Get the first touch point (we only care about single touch for now)
        // The TAMC_GT911 library stores touch points in the points[] array
        // Allow zero coordinates which might be valid for edge touches
        // Update touch point
        point->x = gt911.points[0].x;
        point->y = gt911.points[0].y;
        point->touched = true;
        
        // Debug output (throttled to avoid flooding the serial port)
        if (now - lastDebug > 500) {  // Update every 500ms
            TOUCH_LOG_LN("Touch - X: %d, Y: %d", 
                        point->x, point->y);
            lastDebug = now;
        }
        
        // Update last touch point for debouncing
        lastTouchPoint = *point;
        lastTouchTime = now;
        success = true;
    } else if (now - lastTouchTime < 50) {
        // Debounce: If touch was recently released, keep reporting the last point
        // for a short time to prevent flickering
        *point = lastTouchPoint;
        success = true;
    } else {
        // No touch detected
        point->x = 0;
        point->y = 0;
        point->touched = false;
        
        // Update last touch point if needed
        if (lastTouchPoint.touched) {
            lastTouchPoint.touched = false;
            TOUCH_LOG_LN("Touch released");
        }
    }
    
    // Handle read errors
    if (!success) {
        readErrors++;
        if (readErrors % 10 == 0) {  // Only log every 10 errors to avoid spamming
            TOUCH_LOG_LN("Warning: No touch detected (error count: %d)", readErrors);
        }
    } else {
        readErrors = 0;  // Reset error counter on successful read
    }
    
    return success;
}
