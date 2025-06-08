#include "UIManager.h"
#include <ui.h>
#include <Preferences.h>
#include "debug_config.h"

// Initialize static singleton instance to nullptr
UIManager* UIManager::_instance = nullptr;

// Private constructor implementation
UIManager::UIManager() {
    sgp30 = new Adafruit_SGP30();
    sht31 = new Adafruit_SHT31();
    sgp30Initialized = false;
    sht31Initialized = false;
}

// Singleton instance getter
UIManager* UIManager::getInstance() {
    if (_instance == nullptr) {
        _instance = new UIManager();
    }
    return _instance;
}

// Initialize sensors
bool UIManager::initSensors() {
    bool success = true;
    Preferences preferences;
    
    // Initialize SHT31 temperature and humidity sensor
    // We don't need to call Wire.begin() here as it's already initialized in main.cpp
    if (!sht31->begin(0x44)) {
#if SENSOR_DEBUG
        Serial.println("Couldn't find SHT31 sensor!");
#endif
        success = false;
    } else {
        sht31Initialized = true;
#if SENSOR_DEBUG
        Serial.println("SHT31 sensor initialized successfully");
#endif
    }
    
    // Initialize SGP30 VOC and eCO2 sensor
    // Use the Wire instance that was initialized in main.cpp
    if (!sgp30->begin(&Wire)) {
#if SENSOR_DEBUG
        Serial.println("Couldn't find SGP30 sensor!");
#endif
        success = false;
    } else {
        sgp30Initialized = true;
#if SENSOR_DEBUG
        Serial.println("SGP30 sensor initialized successfully");
        Serial.print("Found SGP30 serial #");
        Serial.print(sgp30->serialnumber[0], HEX);
        Serial.print(sgp30->serialnumber[1], HEX);
        Serial.println(sgp30->serialnumber[2], HEX);
#endif
        // Initialize IAQ algorithm baseline
        sgp30->IAQinit();
        
        // Try to load baseline values from non-volatile storage
        preferences.begin("sgp30", false);
        eco2_baseline = preferences.getUInt("eco2_base", 0);
        tvoc_baseline = preferences.getUInt("tvoc_base", 0);
        preferences.end();
        
        // If we have valid baseline values, set them
        if (eco2_baseline > 0 && tvoc_baseline > 0) {
#if SENSOR_DEBUG
            Serial.printf("Setting SGP30 baselines: eCO2=0x%X, TVOC=0x%X\n", eco2_baseline, tvoc_baseline);
#endif
            sgp30->setIAQBaseline(eco2_baseline, tvoc_baseline);
        }
    }
    
    // Initialize the baseline timestamp
    lastBaselineTime = millis();
    
    return success;
}

// Update temperature display
void UIManager::updateTemperature(float temp) {
    if (ui_tempLabel) {
        // Only update if temperature has changed significantly
        if (abs(lastTemperature - temp) >= 0.1) {
            char buf[16];
            snprintf(buf, sizeof(buf), "%.1f°C", temp);
            lv_label_set_text(ui_tempLabel, buf);
            
            // Set color based on temperature range
            lv_color_t temp_color;
            if (temp < 16) {
                temp_color = lv_color_hex(0x00AFFF); // Blue - cold
            } else if (temp <= 23) {
                temp_color = lv_color_hex(0x00FF00); // Green - comfortable
            } else if (temp <= 26) {
                temp_color = lv_color_hex(0xFF9A00); // Orange - warm
            } else {
                temp_color = lv_color_hex(0xFF0000); // Red - hot
            }
            
            lv_obj_set_style_text_color(ui_tempLabel, temp_color, 0);
            lv_obj_invalidate(ui_tempLabel);
            
            // Update last temperature
            lastTemperature = temp;
            
#if SENSOR_DEBUG
            Serial.printf("Temperature updated: %.1f°C\n", temp);
#endif
        }
    } else {
#if SENSOR_DEBUG
        Serial.println("Temperature label not found in UI");
#endif
    }
}

// Update humidity display
void UIManager::updateHumidity(float humidity) {
    if (ui_humidityLabel) {
        // Only update if humidity has changed significantly
        if (abs(lastHumidity - humidity) >= 1.0) {
            char buf[16];
            snprintf(buf, sizeof(buf), "%.0f%%", humidity);
            lv_label_set_text(ui_humidityLabel, buf);
            
            lv_color_t humidity_color;
            if (humidity < 40) {
                humidity_color = lv_color_hex(0xFFD700); // Yellow - dry
            } else if (humidity <= 60) {
                humidity_color = lv_color_hex(0x00FF00); // Green - optimal
            } else {
                humidity_color = lv_color_hex(0x00AFFF); // Blue - humid
            }
            
            lv_obj_set_style_text_color(ui_humidityLabel, humidity_color, 0);
            lv_obj_invalidate(ui_humidityLabel);
            
            // Update last humidity
            lastHumidity = humidity;
            
#if SENSOR_DEBUG
            Serial.printf("Humidity updated: %.0f%%\n", humidity);
#endif
        }
    } else {
#if SENSOR_DEBUG
        Serial.println("Humidity label not found in UI");
#endif
    }
}

// Update TVOC display
void UIManager::updateTVOC(uint16_t tvoc) {
    if (ui_tvocLabel) {
        // Update if TVOC has changed or if this is the first reading
        if (lastTVOC != tvoc || firstTVOCReading) {
            // Clear first reading flag
            firstTVOCReading = false;
            char buf[16];
            snprintf(buf, sizeof(buf), "%d ppb", tvoc);
            lv_label_set_text(ui_tvocLabel, buf);
            
            lv_color_t tvoc_color;
            if (tvoc < 100) {
                tvoc_color = lv_color_hex(0x00FF00); // Green - excellent
            } else if (tvoc < 300) {
                tvoc_color = lv_color_hex(0xFFD700); // Yellow - good
            } else if (tvoc < 500) {
                tvoc_color = lv_color_hex(0xFF9A00); // Orange - moderate
            } else {
                tvoc_color = lv_color_hex(0xFF0000); // Red - poor
            }
            
            lv_obj_set_style_text_color(ui_tvocLabel, tvoc_color, 0);
            lv_obj_invalidate(ui_tvocLabel);
            
            // Update last TVOC
            lastTVOC = tvoc;
            
#if SENSOR_DEBUG
            Serial.printf("TVOC updated: %d ppb\n", tvoc);
#endif
        }
    } else {
#if SENSOR_DEBUG
        Serial.println("TVOC label not found in UI");
#endif
    }
}

// Update CO2 display
void UIManager::updateCO2(uint16_t eco2) {
    if (ui_eco2Label) {
        // Update if eCO2 has changed or if this is the first reading
        if (lastECO2 != eco2 || firstECO2Reading) {
            // Clear first reading flag
            firstECO2Reading = false;
            char buf[16];
            snprintf(buf, sizeof(buf), "%d ppm", eco2);
            lv_label_set_text(ui_eco2Label, buf);
            
            lv_color_t eco2_color;
            if (eco2 < 800) {
                eco2_color = lv_color_hex(0x00FF00); // Green - excellent
            } else if (eco2 < 1200) {
                eco2_color = lv_color_hex(0xFFD700); // Yellow - good
            } else if (eco2 < 1800) {
                eco2_color = lv_color_hex(0xFF9A00); // Orange - moderate
            } else {
                eco2_color = lv_color_hex(0xFF0000); // Red - poor
            }
            
            lv_obj_set_style_text_color(ui_eco2Label, eco2_color, 0);
            lv_obj_invalidate(ui_eco2Label);
            
            // Update last eCO2
            lastECO2 = eco2;
            
#if SENSOR_DEBUG
            Serial.printf("eCO2 updated: %d ppm\n", eco2);
#endif
        }
    } else {
#if SENSOR_DEBUG
        Serial.println("CO2 label not found in UI");
#endif
    }
}

// Read sensors and update UI
void UIManager::updateEnvironmentalData() {
    // Only proceed if sensors were initialized successfully
    if (sht31Initialized) {
        // Read temperature and humidity from SHT31 (without reinitializing)
        float temperature = sht31->readTemperature();
        float humidity = sht31->readHumidity();
        
        // Check if readings are valid (not NaN)
        if (!isnan(temperature)) {
            updateTemperature(temperature);
        } else {
#if SENSOR_DEBUG
            Serial.println("Invalid temperature reading (NaN)");
#endif
        }
        
        if (!isnan(humidity)) {
            updateHumidity(humidity);
        } else {
#if SENSOR_DEBUG
            Serial.println("Invalid humidity reading (NaN)");
#endif
        }
    }
    
    if (sgp30Initialized) {
        // Read TVOC and eCO2 from SGP30
        if (sgp30->IAQmeasure()) {
            updateTVOC(sgp30->TVOC);
            updateCO2(sgp30->eCO2);
            
            // Handle baseline updates
            handleSGP30Baseline();
        } else {
#if SENSOR_DEBUG
            Serial.println("Failed to read from SGP30 sensor");
#endif
        }
    }
}

// SGP30 baseline management
void UIManager::handleSGP30Baseline() {
    unsigned long currentTime = millis();
    
    // Check if it's time to read/save the baseline (every hour)
    if ((currentTime - lastBaselineTime) >= SGP30_BASELINE_INTERVAL_MS) {
        lastBaselineTime = currentTime;
        
        // Get current baseline values
        if (sgp30->getIAQBaseline(&eco2_baseline, &tvoc_baseline)) {
            // Store the baseline values in non-volatile storage
            Preferences preferences;
            preferences.begin("sgp30", false);
            preferences.putUInt("eco2_base", eco2_baseline);
            preferences.putUInt("tvoc_base", tvoc_baseline);
            preferences.end();
            
#if SENSOR_DEBUG
            Serial.printf("SGP30 baselines saved: eCO2=0x%X, TVOC=0x%X\n", eco2_baseline, tvoc_baseline);
#endif
        } else {
#if SENSOR_DEBUG
            Serial.println("Failed to get SGP30 baseline values");
#endif
        }
    }
}

// Update time on the main screen
void UIManager::updateTimeUI() {
    struct tm timeinfo;
    char timeString[9];
    
    if (getLocalTime(&timeinfo)) {
        strftime(timeString, sizeof(timeString), "%H:%M:%S", &timeinfo);
        
        if (ui_Uhrzeit != NULL) {
            lv_label_set_text(ui_Uhrzeit, timeString);
#if TIME_DEBUG
            Serial.print("Time updated: ");
            Serial.println(timeString);
#endif
        }
    }
}

// Update date on the main screen in German format
void UIManager::updateDateUI() {
    struct tm timeinfo;
    char dateString[30];
    
    if (getLocalTime(&timeinfo)) {
        // Get current hour to track hour changes
        int current_hour = timeinfo.tm_hour;
        
        // Store the last update hour
        hourChangeTracking = current_hour;
        
        // Array of German weekday names
        const char* weekdays_de[] = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};
        
        // Format: Weekday dd.mm.yyyy
        sprintf(dateString, "%s %02d.%02d.%04d", 
                weekdays_de[timeinfo.tm_wday], 
                timeinfo.tm_mday, 
                timeinfo.tm_mon + 1, 
                timeinfo.tm_year + 1900);
        
        if (ui_Datum != NULL) {
            lv_label_set_text(ui_Datum, dateString);
#if TIME_DEBUG
            Serial.print("Date updated: ");
            Serial.println(dateString);
#endif
        }
    }
}

// Check if WiFi status has changed
bool UIManager::hasWiFiStatusChanged() {
    bool currentlyConnected = (WiFi.status() == WL_CONNECTED);
    
    // If connection status changed, we need to update the UI
    if (currentlyConnected != lastConnected) {
        return true;
    }
    
    // If not connected in both cases, nothing to update
    if (!currentlyConnected) {
        return false;
    }
    
    // Connected, check if details changed
    String currentSSID = WiFi.SSID();
    String currentIP = WiFi.localIP().toString();
    int currentRSSI = WiFi.RSSI();
    
    // Check if any relevant WiFi parameter changed
    return (currentSSID != lastSSID || 
            currentIP != lastIP || 
            abs(currentRSSI - lastRSSI) >= 5); // Only update if RSSI changed by 5 dBm or more
}

// Update WiFi status information in the UI
void UIManager::updateWiFiStatusUI() {
    bool currentlyConnected = (WiFi.status() == WL_CONNECTED);
    
    // Check if status has changed to avoid unnecessary updates
    if (!hasWiFiStatusChanged()) {
        return;
    }
    
    // Save current connection state
    lastConnected = currentlyConnected;
    
    // Only update connected information if WiFi is connected
    if (currentlyConnected) {
        // Get current WiFi information
        String ssid = WiFi.SSID();
        String ip = WiFi.localIP().toString();
        int rssi = WiFi.RSSI(); // Signal strength in dBm
        
        // Save current values for future comparison
        lastSSID = ssid;
        lastIP = ip;
        lastRSSI = rssi;
        
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
#if STATUS_DEBUG
        Serial.print("Updating WiFi UI - SSID: ");
        Serial.print(ssid);
        Serial.print(", IP: ");
        Serial.print(ip);
        Serial.print(", Quality: ");
        Serial.println(qualityStr);
#endif
        
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
        
#if STATUS_DEBUG
        Serial.println("WiFi not connected, updated UI accordingly");
#endif
    }
}

// Initialize the weather service with API key and location
bool UIManager::initWeatherService(const String& apiKey, float latitude, float longitude,
                                 const String& units, const String& language) {
    WeatherService& weatherService = WeatherService::getInstance();
    bool result = weatherService.init(apiKey, latitude, longitude, units, language);
    
    // Set the update interval (5 minutes = 300000 ms)
    weatherService.setUpdateInterval(300000);
    
    // Force an update to get initial weather data
    if (result && WiFi.status() == WL_CONNECTED) {
        updateWeatherData();
    }
    
    return result;
}

// Update weather data if needed and update the UI
void UIManager::updateWeatherData() {
    // Check if WiFi is connected - can't update weather without network
    if (WiFi.status() != WL_CONNECTED) {
        #if WEATHER_DEBUG
        Serial.println("Cannot update weather: WiFi not connected");
        #endif
        return;
    }
    
    // Get reference to the singleton weather service
    WeatherService& weatherService = WeatherService::getInstance();
    
    // Check if it's time to update (will only update if interval has passed)
    if (weatherService.update()) {
        // Update was performed, save the timestamp
        lastWeatherUpdateTime = millis();
        
        #if WEATHER_DEBUG
        Serial.println("Weather data updated successfully");
        #endif
    } else {
        #if WEATHER_DEBUG
        Serial.println("No weather update needed yet");
        #endif
    }
}
