#include "ConfigManager.h"

// Initialize static instance pointer
ConfigManager* ConfigManager::instance = nullptr;

ConfigManager::ConfigManager() 
    : configDoc(4096), configLoaded(false) {
}

ConfigManager* ConfigManager::getInstance() {
    if (instance == nullptr) {
        instance = new ConfigManager();
    }
    return instance;
}

bool ConfigManager::initConfig() {
    return loadConfigFromSD();
}

bool ConfigManager::loadConfigFromSD() {
    // Check if config file exists
    if (!SD.exists("/config.json")) {
#if CONFIG_DEBUG
        Serial.println("Error: config.json not found on SD card!");
#endif
        // If config file doesn't exist, create default configuration
        setDefaultConfig();
        
        // Try to save the default config
        if (saveConfig()) {
#if CONFIG_DEBUG
            Serial.println("Created default config.json on SD card");
#endif
            configLoaded = true;
            return true;
        }
        return false;
    }
    
#if CONFIG_DEBUG
    Serial.println("Loading config from SD card...");
#endif
    
    File configFile = SD.open("/config.json", FILE_READ);
    if (!configFile) {
#if CONFIG_DEBUG
        Serial.println("Error: Failed to open config.json on SD card!");
#endif
        return false;
    }
    
    size_t size = configFile.size();
#if CONFIG_DEBUG
    Serial.printf("Successfully opened config.json (%u bytes)\n", size);
#endif
    
    if (size == 0) {
#if CONFIG_DEBUG
        Serial.println("Error: config.json is empty!");
#endif
        configFile.close();
        return false;
    }
    
    // Create a temporary document for parsing
    DynamicJsonDocument tempDoc(4096);
    
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(tempDoc, configFile);
    configFile.close();
    
    if (error) {
#if CONFIG_DEBUG
        Serial.print("Error parsing JSON: ");
        Serial.println(error.c_str());
#endif
        return false;
    }
    
    // Parse the config with defaults for missing elements
    if (!parseConfig(tempDoc)) {
#if CONFIG_DEBUG
        Serial.println("Error parsing configuration values");
#endif
        return false;
    }
    
#if CONFIG_DEBUG
    // For debug, print the parsed JSON document
    Serial.println("Parsed JSON document:");
    serializeJsonPretty(configDoc, Serial);
#endif
    
    configLoaded = true;
    return true;
}

JsonObject ConfigManager::getWiFiConfig() {
    if (!configLoaded || !configDoc.containsKey("wifi")) {
        return JsonObject();
    }
    return configDoc["wifi"].as<JsonObject>();
}

JsonObject ConfigManager::getNTPConfig() {
    if (!configLoaded || !configDoc.containsKey("ntp")) {
        return JsonObject();
    }
    return configDoc["ntp"].as<JsonObject>();
}

JsonObject ConfigManager::getDisplayConfig() {
    if (!configLoaded || !configDoc.containsKey("display")) {
        return JsonObject();
    }
    return configDoc["display"].as<JsonObject>();
}

JsonObject ConfigManager::getWeatherConfig() {
    if (!configLoaded || !configDoc.containsKey("weather")) {
        return JsonObject();
    }
    return configDoc["weather"].as<JsonObject>();
}

JsonObject ConfigManager::getSystemConfig() {
    if (!configLoaded || !configDoc.containsKey("system")) {
        return JsonObject();
    }
    return configDoc["system"].as<JsonObject>();
}

JsonArray ConfigManager::getAlarms() {
    if (!configLoaded || !configDoc.containsKey("alarms")) {
        return JsonArray();
    }
    return configDoc["alarms"].as<JsonArray>();
}

JsonArray ConfigManager::getRadioStations() {
    if (!configLoaded || !configDoc.containsKey("radio_stations")) {
        return JsonArray();
    }
    return configDoc["radio_stations"].as<JsonArray>();
}

String ConfigManager::getFallbackAudio() {
    if (!configLoaded || !configDoc.containsKey("fallback_audio")) {
        return "/alarm.mp3";  // Default fallback
    }
    return configDoc["fallback_audio"].as<String>();
}

bool ConfigManager::getWiFiCredentials(String& ssid, String& password) {
    JsonObject wifiConfig = getWiFiConfig();
    if (wifiConfig.isNull() || !wifiConfig.containsKey("ssid") || !wifiConfig.containsKey("password")) {
        return false;
    }
    
    ssid = wifiConfig["ssid"].as<String>();
    password = wifiConfig["password"].as<String>();
    return !ssid.isEmpty();
}

bool ConfigManager::getNTPSettings(String& server, String& timezone) {
    JsonObject ntpConfig = getNTPConfig();
    if (ntpConfig.isNull()) {
        // Set defaults
        server = "pool.ntp.org";
        timezone = "CET-1CEST,M3.5.0,M10.5.0/3";
        return false;
    }
    
    server = ntpConfig["server"] | "pool.ntp.org";
    timezone = ntpConfig["timezone"] | "CET-1CEST,M3.5.0,M10.5.0/3";
    return true;
}

bool ConfigManager::getWeatherSettings(String& apiKey, float& lat, float& lon, String& units, String& lang) {
    JsonObject weatherConfig = getWeatherConfig();
    if (weatherConfig.isNull()) {
        return false;
    }
    
    apiKey = weatherConfig["appid"].as<String>();
    lat = weatherConfig["lat"] | 0.0f;
    lon = weatherConfig["lon"] | 0.0f;
    units = weatherConfig["units"] | "metric";
    lang = weatherConfig["lang"] | "de";
    
    // Validate configuration
    return !apiKey.isEmpty() && lat != 0.0f && lon != 0.0f;
}

bool ConfigManager::saveConfig() {
#if CONFIG_DEBUG
    Serial.println("Saving config to SD card...");
#endif
    
    // Try to open config file for writing
    File configFile = SD.open("/config.json", FILE_WRITE);
    if (!configFile) {
#if CONFIG_DEBUG
        Serial.println("Error: Failed to open config.json for writing!");
#endif
        return false;
    }
    
    // Serialize JSON to file
    if (serializeJson(configDoc, configFile) == 0) {
#if CONFIG_DEBUG
        Serial.println("Error: Failed to write JSON to config file!");
#endif
        configFile.close();
        return false;
    }
    
    configFile.close();
#if CONFIG_DEBUG
    Serial.println("Config saved successfully");
#endif
    return true;
}

void ConfigManager::setDefaultConfig() {
#if CONFIG_DEBUG
    Serial.println("Creating default configuration...");
#endif
    
    // Create a new empty JSON document to replace any existing config
    configDoc = DynamicJsonDocument(4096);
    
    // WiFi
    JsonObject wifi = configDoc.createNestedObject("wifi");
    wifi["ssid"] = "";
    wifi["password"] = "";
    
    // NTP
    JsonObject ntp = configDoc.createNestedObject("ntp");
    ntp["server"] = "pool.ntp.org";
    ntp["timezone"] = "CET-1CEST,M3.5.0,M10.5.0/3";
    
    // Display
    JsonObject display = configDoc.createNestedObject("display");
    display["brightness"] = 100;
    display["timeout"] = 30;
    display["auto_brightness"] = true;
    display["theme"] = "dark";
    
    // Weather
    JsonObject weather = configDoc.createNestedObject("weather");
    weather["appid"] = "";
    weather["lat"] = 0.0f;
    weather["lon"] = 0.0f;
    weather["units"] = "metric";
    weather["lang"] = "de";
    weather["update_interval"] = 30;
    
    // System
    JsonObject system = configDoc.createNestedObject("system");
    system["hostname"] = "radiowecker";
    system["ota_password"] = "changeme";
    
    // Default alarm
    JsonArray alarms = configDoc.createNestedArray("alarms");
    JsonObject alarm = alarms.createNestedObject();
    alarm["id"] = 1;
    alarm["enabled"] = true;
    alarm["hour"] = 7;
    alarm["minute"] = 0;
    
    JsonArray days = alarm.createNestedArray("days");
    // Monday to Friday true, weekend false
    days.add(false); // Sunday
    days.add(true);  // Monday
    days.add(true);  // Tuesday
    days.add(true);  // Wednesday
    days.add(true);  // Thursday
    days.add(true);  // Friday
    days.add(false); // Saturday
    
    alarm["type"] = "radio";
    alarm["station_id"] = 0;
    alarm["volume"] = 70;
    alarm["fade_in"] = 30;
    alarm["duration"] = 60;
    
    // Default radio station
    JsonArray radio_stations = configDoc.createNestedArray("radio_stations");
    JsonObject station = radio_stations.createNestedObject();
    station["id"] = 0;
    station["name"] = "Example Radio";
    station["url"] = "http://example.com/stream.mp3";
    station["genre"] = "Various";
    
    // Fallback audio
    configDoc["fallback_audio"] = "/alarm.mp3";
}

bool ConfigManager::parseConfig(DynamicJsonDocument& doc) {
#if CONFIG_DEBUG
    Serial.println("Parsing configuration with default values...");
#endif
    
    // Create a new document for our complete config
    configDoc = DynamicJsonDocument(4096);
    
    // Helper function to check if a key exists in the source
    auto hasValidKey = [](const JsonObject& obj, const char* key) {
        return obj.containsKey(key) && !obj[key].isNull();
    };
    
    // WiFi - check if wifi section exists
    JsonObject wifi = configDoc.createNestedObject("wifi");
    if (doc.containsKey("wifi") && doc["wifi"].is<JsonObject>()) {
        JsonObject srcWifi = doc["wifi"].as<JsonObject>();
        wifi["ssid"] = hasValidKey(srcWifi, "ssid") ? srcWifi["ssid"].as<String>() : "";
        wifi["password"] = hasValidKey(srcWifi, "password") ? srcWifi["password"].as<String>() : "";
    } else {
        wifi["ssid"] = "";
        wifi["password"] = "";
    }
    
    // NTP
    JsonObject ntp = configDoc.createNestedObject("ntp");
    if (doc.containsKey("ntp") && doc["ntp"].is<JsonObject>()) {
        JsonObject srcNtp = doc["ntp"].as<JsonObject>();
        ntp["server"] = hasValidKey(srcNtp, "server") ? srcNtp["server"].as<String>() : "pool.ntp.org";
        ntp["timezone"] = hasValidKey(srcNtp, "timezone") ? srcNtp["timezone"].as<String>() : "CET-1CEST,M3.5.0,M10.5.0/3";
    } else {
        ntp["server"] = "pool.ntp.org";
        ntp["timezone"] = "CET-1CEST,M3.5.0,M10.5.0/3";
    }
    
    // Display
    JsonObject display = configDoc.createNestedObject("display");
    if (doc.containsKey("display") && doc["display"].is<JsonObject>()) {
        JsonObject srcDisplay = doc["display"].as<JsonObject>();
        display["brightness"] = hasValidKey(srcDisplay, "brightness") ? srcDisplay["brightness"].as<int>() : 100;
        display["timeout"] = hasValidKey(srcDisplay, "timeout") ? srcDisplay["timeout"].as<int>() : 30;
        display["auto_brightness"] = hasValidKey(srcDisplay, "auto_brightness") ? srcDisplay["auto_brightness"].as<bool>() : true;
        display["theme"] = hasValidKey(srcDisplay, "theme") ? srcDisplay["theme"].as<String>() : "dark";
    } else {
        display["brightness"] = 100;
        display["timeout"] = 30;
        display["auto_brightness"] = true;
        display["theme"] = "dark";
    }
    
    // Weather - much more careful handling to preserve coordinates
    JsonObject weather = configDoc.createNestedObject("weather");
    if (doc.containsKey("weather") && doc["weather"].is<JsonObject>()) {
        JsonObject srcWeather = doc["weather"].as<JsonObject>();
        
        // Carefully preserve the API key and coordinates
        weather["appid"] = hasValidKey(srcWeather, "appid") ? srcWeather["appid"].as<String>() : "";
        weather["lat"] = hasValidKey(srcWeather, "lat") ? srcWeather["lat"].as<float>() : 0.0f;
        weather["lon"] = hasValidKey(srcWeather, "lon") ? srcWeather["lon"].as<float>() : 0.0f;
        weather["units"] = hasValidKey(srcWeather, "units") ? srcWeather["units"].as<String>() : "metric";
        weather["lang"] = hasValidKey(srcWeather, "lang") ? srcWeather["lang"].as<String>() : "de";
        weather["update_interval"] = hasValidKey(srcWeather, "update_interval") ? srcWeather["update_interval"].as<int>() : 30;
    } else {
        weather["appid"] = "";
        weather["lat"] = 0.0f;
        weather["lon"] = 0.0f;
        weather["units"] = "metric";
        weather["lang"] = "de";
        weather["update_interval"] = 30;
    }
    
    // System
    JsonObject system = configDoc.createNestedObject("system");
    system["hostname"] = doc["system"]["hostname"] | "radiowecker";
    system["ota_password"] = doc["system"]["ota_password"] | "changeme";
    
    // Alarms - Handle empty or missing alarms array
    JsonArray alarms = configDoc.createNestedArray("alarms");
    if (doc.containsKey("alarms") && doc["alarms"].is<JsonArray>()) {
        for (JsonObject alarmObj : doc["alarms"].as<JsonArray>()) {
            JsonObject alarm = alarms.createNestedObject();
            alarm["id"] = alarmObj["id"] | 1;
            alarm["enabled"] = alarmObj["enabled"] | false;
            alarm["hour"] = alarmObj["hour"] | 7;
            alarm["minute"] = alarmObj["minute"] | 0;
            
            JsonArray days = alarm.createNestedArray("days");
            if (alarmObj.containsKey("days") && alarmObj["days"].is<JsonArray>()) {
                JsonArray sourceDays = alarmObj["days"].as<JsonArray>();
                // Try to copy existing days
                for (int i = 0; i < 7 && i < sourceDays.size(); i++) {
                    days.add(sourceDays[i] | false);
                }
                // Fill with defaults if needed
                for (int i = days.size(); i < 7; i++) {
                    days.add(i >= 1 && i <= 5); // Weekdays as default
                }
            } else {
                // Add default days (weekdays on)
                days.add(false); // Sunday
                days.add(true);  // Monday
                days.add(true);  // Tuesday
                days.add(true);  // Wednesday
                days.add(true);  // Thursday
                days.add(true);  // Friday
                days.add(false); // Saturday
            }
            
            alarm["type"] = alarmObj["type"] | "radio";
            alarm["station_id"] = alarmObj["station_id"] | 0;
            alarm["volume"] = alarmObj["volume"] | 70;
            alarm["fade_in"] = alarmObj["fade_in"] | 30;
            alarm["duration"] = alarmObj["duration"] | 60;
        }
    }
    
    // If no alarms were found, add a default one
    if (alarms.size() == 0) {
        JsonObject alarm = alarms.createNestedObject();
        alarm["id"] = 1;
        alarm["enabled"] = true;
        alarm["hour"] = 7;
        alarm["minute"] = 0;
        
        JsonArray days = alarm.createNestedArray("days");
        days.add(false); // Sunday
        days.add(true);  // Monday
        days.add(true);  // Tuesday
        days.add(true);  // Wednesday
        days.add(true);  // Thursday
        days.add(true);  // Friday
        days.add(false); // Saturday
        
        alarm["type"] = "radio";
        alarm["station_id"] = 0;
        alarm["volume"] = 70;
        alarm["fade_in"] = 30;
        alarm["duration"] = 60;
    }
    
    // Radio stations - Handle empty or missing stations array
    JsonArray radio_stations = configDoc.createNestedArray("radio_stations");
    if (doc.containsKey("radio_stations") && doc["radio_stations"].is<JsonArray>()) {
        for (JsonObject stationObj : doc["radio_stations"].as<JsonArray>()) {
            JsonObject station = radio_stations.createNestedObject();
            station["id"] = stationObj["id"] | 0;
            station["name"] = stationObj["name"] | "Unknown Station";
            station["url"] = stationObj["url"] | "http://example.com/stream.mp3";
            station["genre"] = stationObj["genre"] | "Various";
        }
    }
    
    // If no stations were found, add a default one
    if (radio_stations.size() == 0) {
        JsonObject station = radio_stations.createNestedObject();
        station["id"] = 0;
        station["name"] = "Example Radio";
        station["url"] = "http://example.com/stream.mp3";
        station["genre"] = "Various";
    }
    
    // Fallback audio
    configDoc["fallback_audio"] = doc["fallback_audio"] | "/alarm.mp3";
    
    return true;
}

void ConfigManager::printConfig() {
#if CONFIG_DEBUG
    if (configLoaded) {
        Serial.println("Current configuration:");
        serializeJsonPretty(configDoc, Serial);
        Serial.println();
    } else {
        Serial.println("No configuration loaded");
    }
#endif
}
