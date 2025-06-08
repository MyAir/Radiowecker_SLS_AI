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
    
    // Read file and parse JSON directly
    DeserializationError error = deserializeJson(configDoc, configFile);
    configFile.close();
    
    if (error) {
#if CONFIG_DEBUG
        Serial.print("Error parsing JSON: ");
        Serial.println(error.c_str());
#endif
        return false;
    }
    
#if CONFIG_DEBUG
    // For debug, print the parsed JSON document
    Serial.println("Parsed JSON document:");
    serializeJsonPretty(configDoc, Serial);
    Serial.println();
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
