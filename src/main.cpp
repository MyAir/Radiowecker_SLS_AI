// Core Arduino and system includes
#include <Arduino.h>
#include <lvgl.h>
#include <Arduino_GFX_Library.h>
#include <Wire.h>
#include "touch.h"

// Forward declarations
void my_log_cb(lv_log_level_t level, const char *buf);
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
void my_touchpad_read(lv_indev_t *drv, lv_indev_data_t *data);
#include <ui.h>

#define GFX_BL 10

/*Don't forget to set Sketchbook location in File/Preferences to the path of your UI project (the parent folder of this INO file)*/

/*Change to your screen resolution*/
static const uint16_t screenWidth  = 800;
static const uint16_t screenHeight = 480;

static lv_display_t *display = nullptr;
static lv_indev_t *touch_indev = nullptr;

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
    
    // Get touch point
    TouchPoint point;
    bool touched = getTouchPoint(&point);
    
    if (touched) {
        // Update last valid point
        last_point = point;
        data->point.x = point.x;
        data->point.y = point.y;
        data->state = LV_INDEV_STATE_PRESSED;
        
        // Debug output (throttled)
        if (Serial && (millis() - last_debug > 100)) {
            Serial.printf("Touch: x=%d, y=%d\n", point.x, point.y);
            last_debug = millis();
        }
    } else {
        // No touch, use last point with released state
        data->point.x = last_point.x;
        data->point.y = last_point.y;
        data->state = LV_INDEV_STATE_RELEASED;
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
    Serial.println("Initializing Touch...");
    touch_init();
    
    // Create an input device for the touch screen
    static lv_indev_t *indev = lv_indev_create();
    if (indev) {
        lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
        lv_indev_set_read_cb(indev, my_touchpad_read);
        touch_indev = indev;
        Serial.println("Touch input device created");
    } else {
        Serial.println("ERROR: Failed to create touch input device");
    }

    // Initialize the UI
    ui_init();

    Serial.println("Setup done");
}

void loop()
{
    static uint32_t last_print = 0;
    uint32_t now = millis();
    
    // Handle LVGL tasks
    lv_timer_handler();
    
    // Periodically print debug info
    if (now - last_print > 1000) {  // Every second
        if (Serial) {
            Serial.printf("Free heap: %d bytes\n", (int)ESP.getFreeHeap());
        }
        last_print = now;
    }
    
    // Small delay to prevent watchdog reset
    delay(2);
}


