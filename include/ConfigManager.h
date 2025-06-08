#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SD.h>
#include <SPI.h>

class ConfigManager {
private:
    static ConfigManager* instance;
    DynamicJsonDocument configDoc;
    bool configLoaded;
    
    ConfigManager();
    bool loadConfigFromSD();
    void setDefaultConfig();
    bool parseConfig(DynamicJsonDocument& doc);
    
public:
    static ConfigManager* getInstance();
    
    // Config loading and access
    bool isConfigLoaded() const { return configLoaded; }
    bool initConfig();
    
    // Config section getters
    JsonObject getWiFiConfig();
    JsonObject getNTPConfig();
    JsonObject getDisplayConfig();
    JsonObject getWeatherConfig();
    JsonObject getSystemConfig();
    JsonArray getAlarms();
    JsonArray getRadioStations();
    String getFallbackAudio();
    
    // Specific config data getters
    bool getWiFiCredentials(String& ssid, String& password);
    bool getNTPSettings(String& server, String& timezone);
    bool getWeatherSettings(String& apiKey, float& lat, float& lon, String& units, String& lang);
    
    // Debug utility
    void printConfig();
    
    // Config management
    bool saveConfig();
};

#endif // CONFIG_MANAGER_H
