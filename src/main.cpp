// Core Arduino and system includes
#include <Arduino.h>
#include <lvgl.h>
#include <Arduino_GFX_Library.h>
#include <Wire.h>
#include "touch.h"

// Use the debug macros from touch.h

// Forward declarations
void my_log_cb(lv_log_level_t level, const char *buf);
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
void my_touchpad_read(lv_indev_t *drv, lv_indev_data_t *data);

// LVGL display and input device
lv_display_t * display = NULL;
lv_indev_t * touch_indev = NULL;

#include <ui.h>

#define GFX_BL 10

/*Don't forget to set Sketchbook location in File/Preferences to the path of your UI project (the parent folder of this INO file)*/

/*Change to your screen resolution*/
static const uint16_t screenWidth  = 800;
static const uint16_t screenHeight = 480;

Arduino_ESP32RGBPanel *bus = new Arduino_ESP32RGBPanel(
        40 /* DE */, 41 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
        45 /* R0 */, 48 /* R1 */, 47 /* R2 */, 21 /* R3 */, 14 /* R4 */,
        5 /* G0 */, 6 /* G1 */, 7 /* G2 */, 15 /* G3 */, 16 /* G4 */, 4 /* G5 */,
        8 /* B0 */, 3 /* B1 */, 46 /* B2 */, 9 /* B3 */, 1 /* B4 */,
        0 /* hsync_polarity */, 8 /* hsync_front_porch */, 4 /* hsync_pulse_width */, 8 /* hsync_back_porch */,
        0 /* vsync_polarity */, 8 /* vsync_front_porch */, 4 /* vsync_pulse_width */, 8 /* vsync_back_porch */,
        1 /* pclk_active_neg */, 16000000 /* prefer_speed */);

Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
    800 /* width */, 480 /* height */, bus, 0 /* rotation */, true /* auto_flush */);

#if LV_USE_LOG != 0
/* LVGL 9.2.2+ logging callback */
void my_log_cb(lv_log_level_t level, const char *buf)
{
    (void)level;  // Unused parameter
    if (Serial) {
        Serial.print("[LVGL] ");
        Serial.print(buf);
        if (buf[strlen(buf)-1] != '\n') {
            Serial.println();
        }
        Serial.flush();
    }
}
#endif

/* Display flushing */
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);
    lv_display_flush_ready(disp);
}

/* Read the touchpad */
void my_touchpad_read(lv_indev_t *drv, lv_indev_data_t *data)
{
    static TouchPoint last_point = {0};
    static uint32_t last_debug = 0;
    static bool last_touched = false;
    
    // Get touch point
    TouchPoint point;
    bool touched = getTouchPoint(point);
    
    // Update LVGL input data
    data->point.x = point.x;
    data->point.y = point.y;
    data->state = point.pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
    
    // Debug output on state change or periodically while touched
    if (Serial) {
        uint32_t now = millis();
        
        // On touch state change
        if (point.pressed != last_touched) {
            if (point.pressed) {
                Serial.printf("Touch BEGAN: x=%d, y=%d\n", point.x, point.y);
            } else {
                Serial.println("Touch ENDED");
            }
            last_touched = point.pressed;
        }
        // Periodic update while touched (every 200ms)
        else if (point.pressed && (now - last_debug > 200)) {
            Serial.printf("Touch HOLD: x=%d, y=%d\n", point.x, point.y);
            last_debug = now;
        }
    }
    
    // Update last point if touched
    if (point.pressed) {
        last_point = point;
    }
}

void setup()
{
    // Initialize USB CDC serial communication
    Serial.begin(115200);
    
    // Wait for USB CDC port to connect or timeout after 3 seconds
    uint32_t startTime = millis();
    while (!Serial && (millis() - startTime < 3000)) {
        delay(10);
    }
    
    // Extra delay to ensure USB CDC is ready
    delay(1000);
    
    Serial.println("\n\n--- Starting Setup ---");
    Serial.println("USB CDC Serial initialized");
    Serial.printf("CPU Frequency: %d MHz\n", getCpuFrequencyMhz());
    Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());

    // Initialize display
    if (Serial) Serial.println("Initializing Display...");
    gfx->begin();
    gfx->fillScreen(BLACK);
    pinMode(GFX_BL, OUTPUT);
    digitalWrite(GFX_BL, HIGH); // Turn on backlight
    if (Serial) Serial.println("Display initialized");
    
    // Print LVGL version
    String LVGL_Arduino = "LVGL v";
    LVGL_Arduino += String(lv_version_major()) + "." + lv_version_minor() + "." + lv_version_patch();
    Serial.println(LVGL_Arduino);

    // Initialize LVGL
    Serial.println("Initializing LVGL...");
    lv_init();
    Serial.println("LVGL initialized");

#if LV_USE_LOG != 0
    lv_log_register_print_cb(my_log_cb);
#endif

    // Initialize display buffer
    Serial.println("Allocating display buffer...");
    static lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * screenWidth * 40, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    if (buf1 == nullptr) {
        Serial.println("ERROR: Failed to allocate display buffer");
        while (1) { delay(100); }
    }
    Serial.println("Display buffer allocated");

    // Initialize the display
    display = lv_display_create(screenWidth, screenHeight);
    lv_display_set_flush_cb(display, my_disp_flush);
    lv_display_set_buffers(display, buf1, NULL, sizeof(lv_color_t) * screenWidth * 40, LV_DISPLAY_RENDER_MODE_PARTIAL);

    // Initialize touch
#if TOUCH_DEBUG
    if (Serial) {
        Serial.println("===== STARTING TOUCH INITIALIZATION =====");
        Serial.println("Initializing touch controller...");
    }
#endif
    touch_init();
    
    // Initialize the input device driver
#if TOUCH_DEBUG
    if (Serial) {
        Serial.println("Initializing LVGL touch input device...");
    }
#endif
    static lv_indev_t * touch_indev = lv_indev_create();
    if (!touch_indev) {
        if (Serial) {
            Serial.println("ERROR: Failed to create LVGL input device!");
        }
    } else {
        lv_indev_set_type(touch_indev, LV_INDEV_TYPE_POINTER);
        lv_indev_set_read_cb(touch_indev, my_touchpad_read);
        
        // Set the display for the input device
        if (display) {
            lv_indev_set_display(touch_indev, display);
#if TOUCH_DEBUG
            Serial.println("Touch input device display set");
#endif
        } else {
            Serial.println("WARNING: Display not available for touch input device");
        }
        
        // Enable the input device
        lv_indev_enable(touch_indev, true);
#if TOUCH_DEBUG
        Serial.println("Touch input device created and configured");
        
        // Verify the input device is enabled
        if (touch_indev) {
            Serial.println("Touch input device is ready");
        } else {
            Serial.println("WARNING: Touch input device is not ready!");
        }
        Serial.println("Touch input device enabled");
#endif
    }

    // Initialize the UI
    ui_init();

    Serial.println("Setup done");
}

void loop()
{
    static uint32_t last_print = 0;
    static uint32_t last_touch_check = 0;
    static bool first_run = true;
    uint32_t now = millis();
    
    // Handle LVGL tasks
    lv_timer_handler();
    
    // Force reinitialize touch if needed (only on first run)
    if (first_run) {
        // If touch is not initialized properly after 3 seconds, try to reinitialize
        if (now > 3000 && !touch_initialized) {
#if TOUCH_DEBUG
            Serial.println("\n===== REINITIALIZING TOUCH CONTROLLER =====\n");
#endif
            touch_init();
        }
        // After 5 seconds, consider first run complete
        if (now > 5000) {
            first_run = false;
        }
    }
    
    // Periodically print debug info
    if (now - last_print > 1000) {  // Every second
        if (Serial) {
#if SYSTEM_DEBUG
            // System debug info
            Serial.printf("Free heap: %d bytes\n", (int)ESP.getFreeHeap());
#endif
            
#if TOUCH_DEBUG
            // Only print touch debug info if TOUCH_DEBUG is enabled
            if (touch_initialized) {
                Serial.println("Touch device is enabled");
                // Try to read touch to keep it active
                TouchPoint point;
                if (getTouchPoint(point)) {
                    if (point.pressed) {
                        Serial.printf("Touch active at X: %d, Y: %d\n", point.x, point.y);
                    }
                }
            } else {
#else
            // If not in debug mode, just silently keep touch active
            if (touch_initialized) {
                TouchPoint point;
                getTouchPoint(point);
            } else {
#endif
#if TOUCH_DEBUG
                Serial.println("Touch device is not initialized!");
#endif
            }
        }
        last_print = now;
    }
    
    // Check for touch input
    if (now - last_touch_check > 100) {  // Every 100ms
        if (touch_indev) {
            // Force a read of the touch device
            lv_indev_get_read_timer(touch_indev);
        }
        last_touch_check = now;
    }
    
    // Small delay to prevent watchdog reset
    delay(2);
}


