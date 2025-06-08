#include "WeatherService.h"
#include "ui.h"
#include <map>

// Forward declaration for getIconForCode method
const void* getIconForCode(const String& iconCode);

// Define static constants
const int WeatherService::MAX_HOURLY_FORECASTS;

// Initialize the static instance pointer
WeatherService* WeatherService::instance = nullptr;

bool WeatherService::init(const String& apiKey, float latitude, float longitude, 
                         const String& unitSystem, const String& language) {
    // Validate parameters
    if (apiKey.isEmpty() || latitude == 0.0f || longitude == 0.0f) {
        #if WEATHER_DEBUG
        Serial.println("Weather service initialization failed: invalid parameters");
        Serial.printf("API Key [%s], Lat [%.6f], Lon [%.6f]\n", 
                    apiKey.isEmpty() ? "EMPTY" : apiKey.c_str(), latitude, longitude);
        #endif
        return false;
    }
    
    // Store configuration parameters
    appid = apiKey;
    lat = latitude;
    lon = longitude;
    units = unitSystem;
    lang = language;
    
    #if WEATHER_DEBUG
    Serial.println("Weather service initialized successfully");
    Serial.printf("API Key: %s\n", appid.c_str());
    Serial.printf("Location: %.6f, %.6f\n", lat, lon);
    Serial.printf("Units: %s, Language: %s\n", units.c_str(), lang.c_str());
    #endif
    
    return true;
}

bool WeatherService::update() {
    uint32_t currentTime = millis();
    
    // Check if this is the first update after boot or if update interval has passed
    if (lastUpdateTime == 0 || currentTime - lastUpdateTime >= updateInterval) {
        #if WEATHER_DEBUG
        if (lastUpdateTime == 0) {
            Serial.println("First weather update after boot");
        } else {
            Serial.println("Weather update interval passed");
        }
        #endif
        return forceUpdate();
    }
    
    return false; // No update needed
}

bool WeatherService::forceUpdate() {
    bool success = fetchWeatherData();
    if (success) {
        lastUpdateTime = millis();
        calculateDailyForecasts();
        updateWeatherUI();
    }
    
    return success;
}

bool WeatherService::fetchWeatherData() {
    if (!WiFi.isConnected()) {
        #if WEATHER_DEBUG
        Serial.println("Cannot fetch weather: WiFi not connected");
        #endif
        return false;
    }
    
    // Check if API key is available
    if (appid.isEmpty() || lat == 0.0f || lon == 0.0f) {
        #if WEATHER_DEBUG
        Serial.println("Weather API configuration is incomplete:");
        Serial.printf("API Key [%s], Lat [%.6f], Lon [%.6f]\n", 
                    appid.isEmpty() ? "EMPTY" : appid.c_str(), lat, lon);
        #endif
        return false;
    }
    
    HTTPClient http;
    String url = "https://api.openweathermap.org/data/3.0/onecall?lat=" + String(lat, 6) +
                "&lon=" + String(lon, 6) +
                "&appid=" + appid +
                "&units=" + units +
                "&lang=" + lang +
                "&exclude=minutely,alerts";
    
    #if WEATHER_DEBUG
    Serial.printf("Fetching weather data from: %s\n", url.c_str());
    #endif
    
    http.begin(url);
    int httpResponseCode = http.GET();
    
    if (httpResponseCode != 200) {
        #if WEATHER_DEBUG
        Serial.printf("HTTP error code: %d\n", httpResponseCode);
        if (httpResponseCode > 0) {
            Serial.printf("Response: %s\n", http.getString().c_str());
        }
        #endif
        http.end();
        return false;
    }
    
    String payload = http.getString();
    http.end();
    
    #if WEATHER_DEBUG
    Serial.println("Weather data fetched successfully");
    //Serial.println(payload); // Only uncomment for debugging - this can be large
    #endif
    
    // Parse JSON response
    DynamicJsonDocument doc(32768); // Adjust size based on your needs
    DeserializationError error = deserializeJson(doc, payload);
    
    if (error) {
        #if WEATHER_DEBUG
        Serial.print("JSON parsing error: ");
        Serial.println(error.c_str());
        #endif
        return false;
    }
    
    // Extract data from JSON
    parseCurrentWeather(doc["current"]);
    parseHourlyForecast(doc["hourly"]);
    
    #if WEATHER_DEBUG
    Serial.println("Weather data parsed successfully");
    #endif
    
    return true;
}

void WeatherService::parseCurrentWeather(const JsonObject& current) {
    currentWeather.dt = current["dt"].as<long>();
    currentWeather.sunrise = current["sunrise"].as<long>();
    currentWeather.sunset = current["sunset"].as<long>();
    currentWeather.temp = current["temp"].as<float>();
    currentWeather.feels_like = current["feels_like"].as<float>();
    currentWeather.pressure = current["pressure"].as<int>();
    currentWeather.humidity = current["humidity"].as<int>();
    currentWeather.dew_point = current["dew_point"].as<float>();
    currentWeather.uvi = current["uvi"].as<float>();
    currentWeather.clouds = current["clouds"].as<int>();
    currentWeather.visibility = current["visibility"].as<int>();
    currentWeather.wind_speed = current["wind_speed"].as<float>();
    currentWeather.wind_deg = current["wind_deg"].as<int>();
    
    // Optional fields that may not be present
    currentWeather.wind_gust = current.containsKey("wind_gust") ? current["wind_gust"].as<float>() : 0;
    currentWeather.rain_1h = 0;
    currentWeather.snow_1h = 0;
    
    if (current.containsKey("rain")) {
        JsonObject rain = current["rain"];
        if (rain.containsKey("1h")) {
            currentWeather.rain_1h = rain["1h"].as<float>();
        }
    }
    
    if (current.containsKey("snow")) {
        JsonObject snow = current["snow"];
        if (snow.containsKey("1h")) {
            currentWeather.snow_1h = snow["1h"].as<float>();
        }
    }
    
    // Weather description and icon
    JsonArray weatherArray = current["weather"];
    if (weatherArray.size() > 0) {
        JsonObject weather = weatherArray[0];
        currentWeather.weather_id = weather["id"].as<int>();
        currentWeather.weather_main = weather["main"].as<const char*>();
        currentWeather.weather_description = weather["description"].as<const char*>();
        currentWeather.weather_icon = weather["icon"].as<const char*>();
    }
    
    #if WEATHER_DEBUG
    Serial.println("Current weather:");
    Serial.printf("  Temperature: %.1f°C\n", currentWeather.temp);
    Serial.printf("  Feels like: %.1f°C\n", currentWeather.feels_like);
    Serial.printf("  Humidity: %d%%\n", currentWeather.humidity);
    Serial.printf("  Weather: %s (%s)\n", currentWeather.weather_main.c_str(), currentWeather.weather_description.c_str());
    Serial.printf("  Icon: %s\n", currentWeather.weather_icon.c_str());
    #endif
}

void WeatherService::parseHourlyForecast(const JsonArray& hourly) {
    hourlyForecastCount = min((int)hourly.size(), MAX_HOURLY_FORECASTS);
    
    for (int i = 0; i < hourlyForecastCount; i++) {
        JsonObject hourData = hourly[i];
        
        hourlyForecasts[i].dt = hourData["dt"].as<long>();
        hourlyForecasts[i].temp = hourData["temp"].as<float>();
        hourlyForecasts[i].feels_like = hourData["feels_like"].as<float>();
        hourlyForecasts[i].pressure = hourData["pressure"].as<int>();
        hourlyForecasts[i].humidity = hourData["humidity"].as<int>();
        hourlyForecasts[i].dew_point = hourData["dew_point"].as<float>();
        hourlyForecasts[i].clouds = hourData["clouds"].as<int>();
        hourlyForecasts[i].visibility = hourData.containsKey("visibility") ? hourData["visibility"].as<int>() : 0;
        hourlyForecasts[i].wind_speed = hourData["wind_speed"].as<float>();
        hourlyForecasts[i].wind_deg = hourData["wind_deg"].as<int>();
        hourlyForecasts[i].wind_gust = hourData.containsKey("wind_gust") ? hourData["wind_gust"].as<float>() : 0;
        hourlyForecasts[i].pop = hourData["pop"].as<float>();
        
        // Rain and snow (optional)
        hourlyForecasts[i].rain_1h = 0;
        hourlyForecasts[i].snow_1h = 0;
        
        if (hourData.containsKey("rain")) {
            if (hourData["rain"].is<JsonObject>()) {
                JsonObject rain = hourData["rain"];
                if (rain.containsKey("1h")) {
                    hourlyForecasts[i].rain_1h = rain["1h"].as<float>();
                }
            } else {
                // Sometimes the API returns rain as a direct value
                hourlyForecasts[i].rain_1h = hourData["rain"].as<float>();
            }
        }
        
        if (hourData.containsKey("snow")) {
            if (hourData["snow"].is<JsonObject>()) {
                JsonObject snow = hourData["snow"];
                if (snow.containsKey("1h")) {
                    hourlyForecasts[i].snow_1h = snow["1h"].as<float>();
                }
            } else {
                hourlyForecasts[i].snow_1h = hourData["snow"].as<float>();
            }
        }
        
        // Weather description and icon
        JsonArray weatherArray = hourData["weather"];
        if (weatherArray.size() > 0) {
            JsonObject weather = weatherArray[0];
            hourlyForecasts[i].weather_id = weather["id"].as<int>();
            hourlyForecasts[i].weather_main = weather["main"].as<const char*>();
            hourlyForecasts[i].weather_description = weather["description"].as<const char*>();
            hourlyForecasts[i].weather_icon = weather["icon"].as<const char*>();
        }
    }
    
    #if WEATHER_DEBUG
    Serial.printf("Parsed %d hourly forecasts\n", hourlyForecastCount);
    #endif
}

void WeatherService::calculateDailyForecasts() {
    if (hourlyForecastCount == 0) {
        #if WEATHER_DEBUG
        Serial.println("No hourly forecasts available for calculation");
        #endif
        return;
    }
    
    // Get current date/time to determine which forecast periods to show
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    int currentHour = timeinfo->tm_hour;
    int currentDay = timeinfo->tm_mday;
    
    #if WEATHER_DEBUG
    Serial.printf("Current time: %02d:%02d on day %d\n", timeinfo->tm_hour, timeinfo->tm_min, currentDay);
    #endif
    
    // Initialize forecast period counters and accumulators
    int morningCount = 0;
    float morningTempSum = 0;
    float morningPopSum = 0;
    std::vector<String> morningIcons;
    
    int afternoonCount = 0;
    float afternoonTempSum = 0;
    float afternoonPopSum = 0;
    std::vector<String> afternoonIcons;
    
    int nightCount = 0;
    float nightTempSum = 0;
    float nightPopSum = 0;
    std::vector<String> nightIcons;
    
    // Determine target days for each period based on current time
    int morningTargetDay, afternoonTargetDay, nightTargetDay;
    
    if (currentHour < 12) {
        // Before noon: show today's morning (if available), afternoon and night
        morningTargetDay = currentDay;
        afternoonTargetDay = currentDay;
        nightTargetDay = currentDay;
    } else if (currentHour < 18) {
        // Afternoon: morning is over, show next day's morning, today's afternoon and night
        morningTargetDay = currentDay + 1;
        afternoonTargetDay = currentDay;
        nightTargetDay = currentDay;
    } else {
        // Evening: morning and afternoon are over, show tomorrow for all periods
        morningTargetDay = currentDay + 1;
        afternoonTargetDay = currentDay + 1;
        nightTargetDay = currentDay + 1;
    }
    
    #if WEATHER_DEBUG
    Serial.printf("Target days - Morning: %d, Afternoon: %d, Night: %d\n", 
                 morningTargetDay, afternoonTargetDay, nightTargetDay);
    #endif
    
    // Process hourly forecasts and categorize them
    for (int i = 0; i < hourlyForecastCount; i++) {
        // Get the date/time for this forecast
        time_t forecastTime = hourlyForecasts[i].dt;
        struct tm* forecastTimeinfo = localtime(&forecastTime);
        int forecastHour = forecastTimeinfo->tm_hour;
        int forecastDay = forecastTimeinfo->tm_mday;
        
        // Only process the next 48 hours
        if (forecastTime > now + 48*3600) {
            break;
        }
        
        // Morning forecasts (6:00 - 11:59) for the morning target day
        if (forecastDay == morningTargetDay && forecastHour >= 6 && forecastHour < 12) {
            morningTempSum += hourlyForecasts[i].temp;
            morningPopSum += hourlyForecasts[i].pop;
            morningIcons.push_back(hourlyForecasts[i].weather_icon);
            morningCount++;
        }
        
        // Afternoon forecasts (12:00 - 17:59) for the afternoon target day
        if (forecastDay == afternoonTargetDay && forecastHour >= 12 && forecastHour < 18) {
            afternoonTempSum += hourlyForecasts[i].temp;
            afternoonPopSum += hourlyForecasts[i].pop;
            afternoonIcons.push_back(hourlyForecasts[i].weather_icon);
            afternoonCount++;
        }
        
        // Night forecasts (18:00 - 23:59) for the night target day
        if (forecastDay == nightTargetDay && forecastHour >= 18) {
            nightTempSum += hourlyForecasts[i].temp;
            nightPopSum += hourlyForecasts[i].pop;
            nightIcons.push_back(hourlyForecasts[i].weather_icon);
            nightCount++;
        }
    }
    
    // Calculate morning forecast summary
    if (morningCount > 0) {
        morningForecast.avgTemp = morningTempSum / morningCount;
        morningForecast.avgPop = morningPopSum / morningCount;
        morningForecast.iconCode = getMostFrequentIcon(morningIcons);
        
        // Ensure icon code is never empty
        if (morningForecast.iconCode.isEmpty()) {
#if WEATHER_DEBUG
            Serial.println("Warning: Morning icon code is empty, using default");
#endif
            morningForecast.iconCode = "01d"; // Default to clear sky day
        }
    }
    
    // Calculate afternoon forecast summary
    if (afternoonCount > 0) {
        afternoonForecast.avgTemp = afternoonTempSum / afternoonCount;
        afternoonForecast.avgPop = afternoonPopSum / afternoonCount;
        afternoonForecast.iconCode = getMostFrequentIcon(afternoonIcons);
        
        // Ensure icon code is never empty
        if (afternoonForecast.iconCode.isEmpty()) {
#if WEATHER_DEBUG
            Serial.println("Warning: Afternoon icon code is empty, using default");
#endif
            afternoonForecast.iconCode = "01d"; // Default to clear sky day
        }
    }
    
    // Calculate night forecast summary
    if (nightCount > 0) {
        nightForecast.avgTemp = nightTempSum / nightCount;
        nightForecast.avgPop = nightPopSum / nightCount;
        nightForecast.iconCode = getMostFrequentIcon(nightIcons);
        
        // Ensure icon code is never empty
        if (nightForecast.iconCode.isEmpty()) {
#if WEATHER_DEBUG
            Serial.println("Warning: Night icon code is empty, using default");
#endif
            nightForecast.iconCode = "01n"; // Default to clear sky night
        }
    }
    
    #if WEATHER_DEBUG
    Serial.println("Daily forecasts calculated:");
    if (morningCount > 0) {
        Serial.printf("  Morning: %.1f°C, %.0f%% rain, icon: %s\n", 
                    morningForecast.avgTemp, morningForecast.avgPop * 100, morningForecast.iconCode.c_str());
    }
    if (afternoonCount > 0) {
        Serial.printf("  Afternoon: %.1f°C, %.0f%% rain, icon: %s\n", 
                    afternoonForecast.avgTemp, afternoonForecast.avgPop * 100, afternoonForecast.iconCode.c_str());
    }
    if (nightCount > 0) {
        Serial.printf("  Night: %.1f°C, %.0f%% rain, icon: %s\n", 
                    nightForecast.avgTemp, nightForecast.avgPop * 100, nightForecast.iconCode.c_str());
    }
    #endif
}

String WeatherService::getMostFrequentIcon(const std::vector<String>& icons) {
    if (icons.empty()) {
#if WEATHER_DEBUG
        Serial.println("Warning: Empty icons vector in getMostFrequentIcon, using default icon");
#endif
        return "01d"; // Default clear sky day icon
    }
    
#if WEATHER_DEBUG
    Serial.printf("Getting most frequent icon from %d icons:\n", icons.size());
    for (size_t i = 0; i < icons.size(); i++) {
        Serial.printf("  Icon %d: %s\n", i, icons[i].c_str());
    }
#endif
    
    // Count occurrences of each icon
    std::map<String, int> iconCount;
    for (const String& icon : icons) {
        // Ensure we're only using the base icon code (without file extension if present)
        String cleanIcon = icon;
        // Remove any file extension that might be in the icon string
        int dotPos = cleanIcon.indexOf('.');
        if (dotPos > 0) {
            cleanIcon = cleanIcon.substring(0, dotPos);
        }
        
        if (iconCount.find(cleanIcon) != iconCount.end()) {
            iconCount[cleanIcon]++;
        } else {
            iconCount[cleanIcon] = 1;
        }
    }
    
    // Find most frequent icon
    String mostFrequentIcon = icons[0];
    int maxCount = 0;
    
#if WEATHER_DEBUG
    Serial.println("Icon counts:");
#endif
    
    for (const auto& pair : iconCount) {
#if WEATHER_DEBUG
        Serial.printf("  %s: %d occurrences\n", pair.first.c_str(), pair.second);
#endif
        if (pair.second > maxCount) {
            maxCount = pair.second;
            mostFrequentIcon = pair.first;
        }
    }
    
#if WEATHER_DEBUG
    Serial.printf("Most frequent icon selected: %s\n", mostFrequentIcon.c_str());
#endif
    
    return mostFrequentIcon;
}

void WeatherService::updateWeatherUI() {
    // Update current weather UI elements
    if (ui_currentTempLabel != nullptr) {
        char tempBuffer[16];
        snprintf(tempBuffer, sizeof(tempBuffer), "%.1f°C", currentWeather.temp);
        lv_label_set_text(ui_currentTempLabel, tempBuffer);
    }
    
    if (ui_feelsLikeLabel != nullptr) {
        char feelsLikeBuffer[32];
        snprintf(feelsLikeBuffer, sizeof(feelsLikeBuffer), "Feels: %.1f°C", currentWeather.feels_like);
        lv_label_set_text(ui_feelsLikeLabel, feelsLikeBuffer);
    }
    
    if (ui_weatherDescLabel != nullptr) {
        lv_label_set_text(ui_weatherDescLabel, currentWeather.weather_description.c_str());
    }
    
    // Update current weather icon
    if (ui_Image1 != nullptr) {
        // Get the corresponding LVGL image based on the icon code
        const void* img_src = getIconForCode(currentWeather.weather_icon);
        if (img_src != nullptr) {
            lv_img_set_src(ui_Image1, img_src);
        }
    }
    
    // Update morning forecast
    if (ui_morningTempLabel != nullptr) {
        char morningTempBuffer[16];
        snprintf(morningTempBuffer, sizeof(morningTempBuffer), "%.1f°C", morningForecast.avgTemp);
        lv_label_set_text(ui_morningTempLabel, morningTempBuffer);
    }
    
    if (ui_morningRainLabel != nullptr) {
        char morningRainBuffer[16];
        snprintf(morningRainBuffer, sizeof(morningRainBuffer), "%.0f%%", morningForecast.avgPop * 100);
        lv_label_set_text(ui_morningRainLabel, morningRainBuffer);
    }
    
    if (ui_morningIcon != nullptr) {
#if WEATHER_DEBUG
        Serial.printf("Setting morning icon to: %s\n", morningForecast.iconCode.c_str());
#endif
        const void* morning_img_src = getIconForCode(morningForecast.iconCode);
        if (morning_img_src != nullptr) {
            lv_img_set_src(ui_morningIcon, morning_img_src);
        }
    }
    
    // Update afternoon forecast
    if (ui_afternoonTempLabel != nullptr) {
        char afternoonTempBuffer[16];
        snprintf(afternoonTempBuffer, sizeof(afternoonTempBuffer), "%.1f°C", afternoonForecast.avgTemp);
        lv_label_set_text(ui_afternoonTempLabel, afternoonTempBuffer);
    }
    
    if (ui_afternoonRainLabel != nullptr) {
        char afternoonRainBuffer[16];
        snprintf(afternoonRainBuffer, sizeof(afternoonRainBuffer), "%.0f%%", afternoonForecast.avgPop * 100);
        lv_label_set_text(ui_afternoonRainLabel, afternoonRainBuffer);
    }
    
    if (ui_afternoonIcon != nullptr) {
#if WEATHER_DEBUG
        Serial.printf("Setting afternoon icon to: %s\n", afternoonForecast.iconCode.c_str());
#endif
        const void* afternoon_img_src = getIconForCode(afternoonForecast.iconCode);
        if (afternoon_img_src != nullptr) {
            lv_img_set_src(ui_afternoonIcon, afternoon_img_src);
        }
    }
    
    // Update night forecast
    if (ui_nightTempLabel != nullptr) {
        char nightTempBuffer[16];
        snprintf(nightTempBuffer, sizeof(nightTempBuffer), "%.1f°C", nightForecast.avgTemp);
        lv_label_set_text(ui_nightTempLabel, nightTempBuffer);
    }
    
    if (ui_nightRainLabel != nullptr) {
        char nightRainBuffer[16];
        snprintf(nightRainBuffer, sizeof(nightRainBuffer), "%.0f%%", nightForecast.avgPop * 100);
        lv_label_set_text(ui_nightRainLabel, nightRainBuffer);
    }
    
    if (ui_nightIcon != nullptr) {
#if WEATHER_DEBUG
        Serial.printf("Setting night icon to: %s\n", nightForecast.iconCode.c_str());
#endif
        const void* night_img_src = getIconForCode(nightForecast.iconCode);
        if (night_img_src != nullptr) {
            lv_img_set_src(ui_nightIcon, night_img_src);
        }
    }
    
    #if WEATHER_DEBUG
    Serial.println("Weather UI updated");
    #endif
}

// Implementation of getIconForCode function to map OpenWeatherMap icon codes to UI icon resources
const void* getIconForCode(const String& iconCode) {
    // Map OpenWeatherMap icon codes to our UI icon resources
    if (iconCode == "01d") return &ui_img_weather_icons_01d_png;
    if (iconCode == "01n") return &ui_img_weather_icons_01n_png;
    if (iconCode == "02d") return &ui_img_weather_icons_02d_png;
    if (iconCode == "02n") return &ui_img_weather_icons_02n_png;
    if (iconCode == "03d") return &ui_img_weather_icons_03d_png;
    if (iconCode == "03n") return &ui_img_weather_icons_03n_png;
    if (iconCode == "04d") return &ui_img_weather_icons_04d_png;
    if (iconCode == "04n") return &ui_img_weather_icons_04n_png;
    if (iconCode == "09d") return &ui_img_weather_icons_09d_png;
    if (iconCode == "09n") return &ui_img_weather_icons_09n_png;
    if (iconCode == "10d") return &ui_img_weather_icons_10d_png;
    if (iconCode == "10n") return &ui_img_weather_icons_10n_png;
    if (iconCode == "11d") return &ui_img_weather_icons_11d_png;
    if (iconCode == "11n") return &ui_img_weather_icons_11n_png;
    if (iconCode == "13d") return &ui_img_weather_icons_13d_png;
    if (iconCode == "13n") return &ui_img_weather_icons_13n_png;
    if (iconCode == "50d") return &ui_img_weather_icons_50d_png;
    if (iconCode == "50n") return &ui_img_weather_icons_50n_png;
    
    // Default case - clear sky day if no match is found
    return &ui_img_weather_icons_01d_png;
}
