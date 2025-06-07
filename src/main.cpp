// Core Arduino and system includes
#include <Arduino.h>
#include <lvgl.h>
#include <Arduino_GFX_Library.h>
#include <SPIFFS.h>
#include <SD.h>
#include <SPI.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <time.h>
#include "touch.h"

// Use the debug macros from touch.h

// Forward declarations
void my_log_cb(lv_log_level_t level, const char *buf);
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
void my_touchpad_read(lv_indev_t *drv, lv_indev_data_t *data);
void listDirectory(fs::FS &fs, const char *dirname, uint8_t levels);
void connectToWiFi();
void syncTimeFromNTP(DynamicJsonDocument& config);
void updateWiFiStatusUI();
void startPeriodicTasks();

// LVGL display and input device
lv_display_t * display = NULL;
lv_indev_t * touch_indev = NULL;

// Timer for periodic WiFi status updates
lv_timer_t * wifi_status_timer = NULL;

// Variable to store last WiFi status update time
unsigned long lastWiFiUpdateTime = 0;

#include <ui.h>

#define GFX_BL 44

// SD Card pins
#define SD_SCK  12
#define SD_MISO 13
#define SD_MOSI 11
#define SD_CS   10

// LVGL filesystem drive letter
#define DRIVE_LETTER 'S'

// Forward declare SD card filesystem driver functions for LVGL
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);

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

// List directory contents on SD card with indentation for nested folders
void listDirectory(fs::FS &fs, const char *dirname, uint8_t levels) {
    File root = fs.open(dirname);
    if (!root) {
        Serial.println("Failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        // Create proper indentation based on directory level
        for (uint8_t i = 0; i < levels; i++) {
            Serial.print("  ");
        }
        
        Serial.print(file.name());
        if (file.isDirectory()) {
            Serial.println("/");
            if (levels < 3) { // Limit recursion to 3 levels deep
                listDirectory(fs, file.path(), levels + 1);
            }
        } else {
            // Print file size
            Serial.print("  (");
            Serial.print(file.size());
            Serial.println(" bytes)");
        }
        file = root.openNextFile();
    }
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
#if TOUCH_DEBUG
            if (point.pressed) {
                Serial.printf("Touch BEGAN: x=%d, y=%d\n", point.x, point.y);
            } else {
                Serial.println("Touch ENDED");
            }
#endif
            last_touched = point.pressed;
        }
        // Periodic update while touched (every 200ms)
        else if (point.pressed && (now - last_debug > 200)) {
#if TOUCH_DEBUG
            Serial.printf("Touch HOLD: x=%d, y=%d\n", point.x, point.y);
#endif
            last_debug = now;
        }
    }
    
    // Update last point if touched
    if (point.pressed) {
        last_point = point;
    }
}

/*use Arduinos millis() as tick source*/
static uint32_t my_tick(void)
{
    return millis();
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
    digitalWrite(GFX_BL, LOW); // Turn on backlight
    if (Serial) Serial.println("Display initialized");
    
    // Print LVGL version
    String LVGL_Arduino = "LVGL v";
    LVGL_Arduino += String(lv_version_major()) + "." + lv_version_minor() + "." + lv_version_patch();
    Serial.println(LVGL_Arduino);

    // Initialize LVGL
    Serial.println("Initializing LVGL...");
    lv_init();
    /*Set a tick source so that LVGL will know how much time elapsed. */
    lv_tick_set_cb(my_tick);

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

    // Initialize SD Card
    Serial.println("Initializing SD card...");
    SPI.begin(SD_SCK, SD_MISO, SD_MOSI);
    if (!SD.begin(SD_CS)) {
        Serial.println("SD Card initialization failed!");
    } else {
        Serial.println("SD Card initialization successful!");
        
        // Initialize LVGL filesystem driver for SD card
        static lv_fs_drv_t fs_drv;
        lv_fs_drv_init(&fs_drv);

        // Set up driver letter and callbacks
        fs_drv.letter = DRIVE_LETTER;
        fs_drv.cache_size = 0;
        
        fs_drv.open_cb = fs_open;
        fs_drv.close_cb = fs_close;
        fs_drv.read_cb = fs_read;
        fs_drv.seek_cb = fs_seek;
        fs_drv.tell_cb = fs_tell;

        // Register the driver
        lv_fs_drv_register(&fs_drv);
        Serial.printf("LVGL filesystem driver registered with letter '%c:'", DRIVE_LETTER);
        Serial.println();
        
        // Connect to WiFi using credentials from config.json
        connectToWiFi();
        
#if SD_DEBUG
        // List files on SD card
        Serial.println("Files on SD Card:");
        listDirectory(SD, "/", 0);
#endif
    }

    // Initialize the UI
    ui_init();
    Serial.println("UI initialized");
    
    // Start periodic tasks (WiFi status updates, etc.)
    startPeriodicTasks();

    Serial.println("Setup done");
}

// Connect to WiFi using credentials from config.json on SD card
void connectToWiFi() {
    Serial.println("Reading WiFi credentials from config.json...");
    
    // Check if config file exists on SD card
    if (!SD.exists("/config.json")) {
        Serial.println("Error: config.json not found on SD card!");
        return;
    }
    
    // Open config.json file
    File configFile = SD.open("/config.json", FILE_READ);
    if (!configFile) {
        Serial.println("Error: Failed to open config.json");
        return;
    }
    
    // Allocate a buffer to store contents of the file
    // We need a larger capacity for the JSON document - 4KB should be plenty
    const size_t capacity = 4096;
    DynamicJsonDocument doc(capacity);
    
    // Parse JSON
    DeserializationError error = deserializeJson(doc, configFile);
    configFile.close();
    
    // Check for parsing errors
    if (error) {
        Serial.print("Error parsing JSON: ");
        Serial.println(error.c_str());
        return;
    }
    
    // Extract WiFi credentials
    const char* ssid = doc["wifi"]["ssid"];
    const char* password = doc["wifi"]["password"];
    
    if (!ssid || !password) {
        Serial.println("Error: WiFi credentials not found in config.json");
        return;
    }
    
    // Connect to WiFi
    Serial.print("Connecting to WiFi SSID: ");
    Serial.println(ssid);
    
    WiFi.begin(ssid, password);
    
    // Wait for connection with timeout
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.print("WiFi connected! IP address: ");
        Serial.println(WiFi.localIP());
        
        // Time sync after WiFi is connected
        syncTimeFromNTP(doc);
    } else {
        Serial.println();
        Serial.println("WiFi connection failed!");
    }
}

// NTP time synchronization function
void syncTimeFromNTP(DynamicJsonDocument& config) {
    Serial.println("Synchronizing time from NTP...");
    
    // Extract NTP server and timezone from config
    const char* ntpServer1 = config["ntp"]["server1"] | "pool.ntp.org";
    const char* ntpServer2 = config["ntp"]["server2"] | "time.nist.gov";
    const char* timezone = config["ntp"]["timezone"] | "CET-1CEST,M3.5.0,M10.5.0/3";
    
    Serial.print("NTP Server 1: ");
    Serial.println(ntpServer1);
    Serial.print("NTP Server 2: ");
    Serial.println(ntpServer2);
    Serial.print("Timezone: ");
    Serial.println(timezone);
    
    // Configure time with NTP servers and timezone
    configTzTime(timezone, ntpServer1, ntpServer2);
    
    // Wait for time to be set
    struct tm timeinfo;
    int retry = 0;
    const int maxRetries = 10;
    
    Serial.println("Waiting for NTP synchronization");
    while (!getLocalTime(&timeinfo) && retry < maxRetries) {
        Serial.print(".");
        delay(1000);
        retry++;
    }
    
    if (retry < maxRetries) {
        Serial.println();
        Serial.println("Time synchronized!");
        
        // Display the current time
        char timeStr[64];
        strftime(timeStr, sizeof(timeStr), "%A, %B %d %Y %H:%M:%S", &timeinfo);
        Serial.print("Current time: ");
        Serial.println(timeStr);
    } else {
        Serial.println();
        Serial.println("Failed to synchronize time!");
    }
}

// LVGL Filesystem Driver Functions

static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode) {
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;
    void * f = NULL;
    
    if(mode == LV_FS_MODE_RD) {
        // Remove drive letter from path (e.g. S:/foo.txt -> /foo.txt)
        if(path[0] == DRIVE_LETTER && path[1] == ':') {
            path += 2;
        }
        
        // Open the file for reading
        File file = SD.open(path);
        if(file) {
            // Allocate memory to store File object pointer
            f = malloc(sizeof(File));
            if(f) {
                memcpy(f, &file, sizeof(File));
                res = LV_FS_RES_OK;
            } else {
                file.close();
            }
        }
    }
    
    if(res != LV_FS_RES_OK) {
        return NULL;
    }
    return f;
}

static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p) {
    if(file_p) {
        File * fp = (File*)file_p;
        fp->close();
        free(file_p);
    }
    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br) {
    if(file_p == NULL) return LV_FS_RES_INV_PARAM;
    
    File * fp = (File*)file_p;
    *br = fp->read((uint8_t*)buf, btr);
    return (*br <= 0) ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence) {
    if(file_p == NULL) return LV_FS_RES_INV_PARAM;
    
    File * fp = (File*)file_p;
    
    SeekMode mode;
    if(whence == LV_FS_SEEK_SET) {
        mode = SeekSet;
    } else if(whence == LV_FS_SEEK_CUR) {
        mode = SeekCur;
    } else if(whence == LV_FS_SEEK_END) {
        mode = SeekEnd;
    } else {
        return LV_FS_RES_INV_PARAM;
    }
    
    if(fp->seek(pos, mode)) {
        return LV_FS_RES_OK;
    } else {
        return LV_FS_RES_UNKNOWN;
    }
}

static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p) {
    if(file_p == NULL) return LV_FS_RES_INV_PARAM;
    
    File * fp = (File*)file_p;
    *pos_p = fp->position();
    return LV_FS_RES_OK;
}

// Update WiFi status information in the UI
void updateWiFiStatusUI() {
    // Only update if WiFi is connected
    if (WiFi.status() == WL_CONNECTED) {
        // Get current WiFi information
        String ssid = WiFi.SSID();
        String ip = WiFi.localIP().toString();
        int rssi = WiFi.RSSI(); // Signal strength in dBm
        
        // Convert RSSI to quality percentage (typically, -50dBm is excellent, -100dBm is poor)
        int quality = constrain(map(rssi, -100, -50, 0, 100), 0, 100);
        
        // Create quality string with WiFi symbol and color based on quality
        String colorTag;
        
        // Color gradient from red to yellow to green based on quality
        if (quality < 30) {
            // Poor signal - red
            colorTag = "#FF0000 ";
        } else if (quality < 50) {
            // Weak signal - orange
            colorTag = "#FF8000 ";
        } else if (quality < 70) {
            // Medium signal - yellow
            colorTag = "#FFFF00 ";
        } else {
            // Good signal - green
            colorTag = "#00FF00 ";
        }
        
        // Format with LVGL color text: #RRGGBB followed by the text
        String qualityStr = String(LV_SYMBOL_WIFI) + " " + colorTag + String(quality) + "%";
        
        // Debug output
        Serial.print("Updating WiFi UI - SSID: ");
        Serial.print(ssid);
        Serial.print(", IP: ");
        Serial.print(ip);
        Serial.print(", Quality: ");
        Serial.println(qualityStr);
        
        // Update the UI labels
        if (ui_wifiLabel != NULL) {
            lv_label_set_text(ui_wifiLabel, ssid.c_str());
        }
        
        if (ui_ipLabel != NULL) {
            lv_label_set_text(ui_ipLabel, ip.c_str());
        }
        
        if (ui_wifiQualityLabel != NULL) {
            lv_label_set_text(ui_wifiQualityLabel, qualityStr.c_str());
        }
        
        lastWiFiUpdateTime = millis();
    } else {
        // WiFi is not connected - update UI accordingly
        if (ui_wifiLabel != NULL) {
            lv_label_set_text(ui_wifiLabel, "Not Connected");
        }
        
        if (ui_ipLabel != NULL) {
            lv_label_set_text(ui_ipLabel, "--.--.--.--");
        }
        
        if (ui_wifiQualityLabel != NULL) {
            // Show disconnected WiFi in red
            lv_label_set_text(ui_wifiQualityLabel, LV_SYMBOL_WIFI "  #FF0000 --");
        }
    }
}

// WiFi status update timer callback
void wifiStatusTimerCallback(lv_timer_t * timer) {
    updateWiFiStatusUI();
}

// Start all periodic tasks
void startPeriodicTasks() {
    // Enable recoloring on the WiFi quality label to support colored text
    if (ui_wifiQualityLabel != NULL) {
        // Enable label recoloring to support HTML-style color tags
        lv_label_set_recolor(ui_wifiQualityLabel, true);
    }
    
    // Create a timer that runs every 10 seconds (10000ms) for WiFi status updates
    wifi_status_timer = lv_timer_create(wifiStatusTimerCallback, 10000, NULL);
    
    // Run the first update immediately
    updateWiFiStatusUI();
}

void loop()
{
    static uint32_t last_print = 0;
    static uint32_t last_touch_check = 0;
    static bool first_run = true;
    static uint32_t last_tick = 0;
    uint32_t now = millis();
    
    // Update LVGL tick counter - required for proper timing
    if(now - last_tick > 0) {
        lv_tick_inc(now - last_tick);
        last_tick = now;
    }
    
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
