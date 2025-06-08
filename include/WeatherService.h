#ifndef WEATHERSERVICE_H
#define WEATHERSERVICE_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <time.h>
#include "debug_config.h"

// Define debug flag for weather service
#ifndef WEATHER_DEBUG
  #define WEATHER_DEBUG 0
#endif

class WeatherService {
public:
    // Constants
    static const int MAX_HOURLY_FORECASTS = 48;  // Maximum number of hourly forecasts (2 days worth)
    
    // Weather data structures
    // Morning, afternoon, and night forecast summary
    struct ForecastSummary {
        float avgTemp = 0.0f;
        float avgPop = 0.0f;   // Average probability of precipitation
        String iconCode;       // Most frequent icon code
    };
    
    struct CurrentWeather {
        long dt = 0;           // Unix timestamp
        long sunrise = 0;
        long sunset = 0;
        float temp = 0.0f;
        float feels_like = 0.0f;
        int pressure = 0;
        int humidity = 0;
        float dew_point = 0.0f;
        int clouds = 0;
        float uvi = 0.0f;
        int visibility = 0;
        float wind_speed = 0.0f;
        float wind_gust = 0.0f;
        int wind_deg = 0;
        float rain_1h = 0.0f;
        float snow_1h = 0.0f;
        int weather_id = 0;
        String weather_main;
        String weather_description;
        String weather_icon;
    };
    
    // Structure for hourly forecast
    struct HourlyForecast {
        long dt = 0;         // Unix timestamp for this hour
        float temp = 0.0f;   // Temperature
        float feels_like = 0.0f;
        int pressure = 0;
        int humidity = 0;
        float dew_point = 0.0f;
        int clouds = 0;
        float uvi = 0.0f;
        int visibility = 0;
        float wind_speed = 0.0f;
        float wind_gust = 0.0f;
        int wind_deg = 0;
        float pop = 0.0f;    // Probability of precipitation
        float rain_1h = 0.0f;
        float snow_1h = 0.0f;
        int weather_id = 0;
        String weather_main;
        String weather_description;
        String weather_icon;
    };
    
private:
    static WeatherService* instance;
    
    // Private constructor
    WeatherService() : appid(""), lat(0.0f), lon(0.0f), units("metric"), lang("de"), lastUpdateTime(0) {}
    
    // Prevent copying and assignment
    WeatherService(const WeatherService&) = delete;
    WeatherService& operator=(const WeatherService&) = delete;
    
    // API call details
    String appid;
    float lat = 0.0f;
    float lon = 0.0f;
    String units = "metric";
    String lang = "de";
    uint32_t lastUpdateTime = 0;
    uint32_t updateInterval = 300000; // Default 5 minutes (300,000 ms)
    
    // Weather data storage
    CurrentWeather currentWeather;
    
    // Store hourly forecasts
    HourlyForecast hourlyForecasts[MAX_HOURLY_FORECASTS];
    int hourlyForecastCount = 0;
    
    // Storage for morning, afternoon, and night forecast summaries
    ForecastSummary morningForecast;
    ForecastSummary afternoonForecast;
    ForecastSummary nightForecast;
    
    // Function to make API call
    bool fetchWeatherData();
    
    // Parse different parts of the API response
    void parseCurrentWeather(const JsonObject& current);
    void parseHourlyForecast(const JsonArray& hourly);
    
    // Calculate morning, afternoon, and night forecasts based on hourly data
    void calculateDailyForecasts();
    String getMostFrequentIcon(const std::vector<String>& icons);

public:
    // Static method to get the singleton instance
    static WeatherService& getInstance() {
        if (!instance) {
            instance = new WeatherService();
        }
        return *instance;
    }
    
    // Initialize with API key and location
    bool init(const String& apiKey, float latitude, float longitude, 
              const String& unitSystem = "metric", const String& language = "de");
    
    // Update weather data (will only fetch if updateInterval has passed)
    bool update();
    
    // Force update regardless of time interval
    bool forceUpdate();
    
    // Getters for weather data
    const CurrentWeather& getCurrentWeather() const { return currentWeather; }
    
    // Get forecast summaries
    const ForecastSummary& getMorningForecast() const { return morningForecast; }
    const ForecastSummary& getAfternoonForecast() const { return afternoonForecast; }
    const ForecastSummary& getNightForecast() const { return nightForecast; }
    
    // Update UI with weather data
    void updateWeatherUI();
    
    // Set update interval (in milliseconds)
    void setUpdateInterval(uint32_t interval) { updateInterval = interval; }
};

#endif // WEATHERSERVICE_H
