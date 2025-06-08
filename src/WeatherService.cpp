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
    
    // Get current time
    time_t now;
    time(&now);
    
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    int currentHour = timeinfo.tm_hour;
    int currentDay = timeinfo.tm_mday;
    int currentMonth = timeinfo.tm_mon + 1; // 0-based to 1-based
    
    // Get sunrise and sunset times from current weather data
    time_t sunriseTime = currentWeather.sunrise;
    time_t sunsetTime = currentWeather.sunset;
    
    struct tm sunriseInfo, sunsetInfo;
    localtime_r(&sunriseTime, &sunriseInfo);
    localtime_r(&sunsetTime, &sunsetInfo);
    
    int sunriseHour = sunriseInfo.tm_hour;
    int sunsetHour = sunsetInfo.tm_hour;
    
    #if WEATHER_DEBUG
    Serial.printf("Current time: %02d:%02d, day: %02d.%02d\n", 
                 currentHour, timeinfo.tm_min, currentDay, currentMonth + 1);
    Serial.printf("Sunrise: %02d:%02d, Sunset: %02d:%02d\n", 
                 sunriseHour, sunriseInfo.tm_min, 
                 sunsetHour, sunsetInfo.tm_min);
    #endif
    
    int middayHour = 12;
    
    // Initialize variables for calculating averages
    float morningTempSum = 0;
    float morningPopSum = 0;
    float afternoonTempSum = 0;
    float afternoonPopSum = 0;
    float nightTempSum = 0;
    float nightPopSum = 0;
    
    int morningCount = 0;
    int afternoonCount = 0;
    int nightCount = 0;
    
    // Store weather icons to find the most frequent one
    std::vector<String> morningIcons;
    std::vector<String> afternoonIcons;
    std::vector<String> nightIcons;
    
    // Define time ranges for each period based on current time
    time_t morningStartTime, morningEndTime;
    time_t afternoonStartTime, afternoonEndTime;
    time_t nightStartTime, nightEndTime;
    
    // Set start and end days/times for each period based on current time
    const int NOON_HOUR = 12;
    
    if (currentHour >= sunriseHour && currentHour < NOON_HOUR) {
        // Current time is in the morning (after sunrise, before noon)
        #if WEATHER_DEBUG
        Serial.println("Current time is in the morning period");
        #endif
        
        // Morning: now until noon today
        morningStartTime = now;
        
        // Calculate noon today
        struct tm noonTime = timeinfo;
        noonTime.tm_hour = NOON_HOUR;
        noonTime.tm_min = 0;
        noonTime.tm_sec = 0;
        morningEndTime = mktime(&noonTime);
        
        // Afternoon: noon today until sunset today
        afternoonStartTime = morningEndTime;
        afternoonEndTime = sunsetTime;
        
        // Night: sunset today until sunrise tomorrow
        nightStartTime = sunsetTime;
        
        // Calculate sunrise tomorrow
        struct tm tomorrowSunrise = sunriseInfo;
        tomorrowSunrise.tm_mday++; // Next day
        nightEndTime = mktime(&tomorrowSunrise);
        
    } else if (currentHour >= NOON_HOUR && currentHour < sunsetHour) {
        // Current time is in the afternoon (after noon, before sunset)
        #if WEATHER_DEBUG
        Serial.println("Current time is in the afternoon period");
        #endif
        
        // Calculate sunrise tomorrow
        struct tm tomorrowSunrise = sunriseInfo;
        tomorrowSunrise.tm_mday++; // Next day
        time_t nextSunrise = mktime(&tomorrowSunrise);
        
        // Calculate noon tomorrow
        struct tm tomorrowNoon = timeinfo;
        tomorrowNoon.tm_mday++; // Next day
        tomorrowNoon.tm_hour = NOON_HOUR;
        tomorrowNoon.tm_min = 0;
        tomorrowNoon.tm_sec = 0;
        time_t nextNoon = mktime(&tomorrowNoon);
        
        // Morning: sunrise tomorrow until noon tomorrow
        morningStartTime = nextSunrise;
        morningEndTime = nextNoon;
        
        // Afternoon: now until sunset today
        afternoonStartTime = now;
        afternoonEndTime = sunsetTime;
        
        // Night: sunset today until sunrise tomorrow
        nightStartTime = sunsetTime;
        nightEndTime = nextSunrise;
        
    } else if (currentHour >= sunsetHour) {
        // Current time is at night after sunset but before midnight
        #if WEATHER_DEBUG
        Serial.println("Current time is at night (after sunset, before midnight)");
        #endif
        
        // Calculate sunrise tomorrow
        struct tm tomorrowSunrise = sunriseInfo;
        tomorrowSunrise.tm_mday++; // Next day
        time_t nextSunrise = mktime(&tomorrowSunrise);
        
        // Calculate noon tomorrow
        struct tm tomorrowNoon = timeinfo;
        tomorrowNoon.tm_mday++; // Next day
        tomorrowNoon.tm_hour = NOON_HOUR;
        tomorrowNoon.tm_min = 0;
        tomorrowNoon.tm_sec = 0;
        time_t nextNoon = mktime(&tomorrowNoon);
        
        // Calculate sunset tomorrow
        struct tm tomorrowSunset = sunsetInfo;
        tomorrowSunset.tm_mday++; // Next day
        time_t nextSunset = mktime(&tomorrowSunset);
        
        // Morning: sunrise tomorrow until noon tomorrow
        morningStartTime = nextSunrise;
        morningEndTime = nextNoon;
        
        // Afternoon: noon tomorrow until sunset tomorrow
        afternoonStartTime = nextNoon;
        afternoonEndTime = nextSunset;
        
        // Night: now until sunrise tomorrow
        nightStartTime = now;
        nightEndTime = nextSunrise;
        
    } else {
        // Current time is after midnight and before sunrise
        #if WEATHER_DEBUG
        Serial.println("Current time is at night (after midnight, before sunrise)");
        #endif
        
        // Calculate noon today
        struct tm todayNoon = timeinfo;
        todayNoon.tm_hour = NOON_HOUR;
        todayNoon.tm_min = 0;
        todayNoon.tm_sec = 0;
        time_t noonToday = mktime(&todayNoon);
        
        // Morning: sunrise today until noon today
        morningStartTime = sunriseTime;
        morningEndTime = noonToday;
        
        // Afternoon: noon today until sunset today
        afternoonStartTime = noonToday;
        afternoonEndTime = sunsetTime;
        
        // Night: now until sunrise today
        nightStartTime = now;
        nightEndTime = sunriseTime;
    }
    
    #if WEATHER_DEBUG
    struct tm morningStart, morningEnd, afternoonStart, afternoonEnd, nightStart, nightEnd;
    localtime_r(&morningStartTime, &morningStart);
    localtime_r(&morningEndTime, &morningEnd);
    localtime_r(&afternoonStartTime, &afternoonStart);
    localtime_r(&afternoonEndTime, &afternoonEnd);
    localtime_r(&nightStartTime, &nightStart);
    localtime_r(&nightEndTime, &nightEnd);
    
    Serial.printf("Morning period: %02d.%02d %02d:%02d to %02d.%02d %02d:%02d\n",
                 morningStart.tm_mday, morningStart.tm_mon + 1, morningStart.tm_hour, morningStart.tm_min,
                 morningEnd.tm_mday, morningEnd.tm_mon + 1, morningEnd.tm_hour, morningEnd.tm_min);
    
    Serial.printf("Afternoon period: %02d.%02d %02d:%02d to %02d.%02d %02d:%02d\n",
                 afternoonStart.tm_mday, afternoonStart.tm_mon + 1, afternoonStart.tm_hour, afternoonStart.tm_min,
                 afternoonEnd.tm_mday, afternoonEnd.tm_mon + 1, afternoonEnd.tm_hour, afternoonEnd.tm_min);
    
    Serial.printf("Night period: %02d.%02d %02d:%02d to %02d.%02d %02d:%02d\n",
                 nightStart.tm_mday, nightStart.tm_mon + 1, nightStart.tm_hour, nightStart.tm_min,
                 nightEnd.tm_mday, nightEnd.tm_mon + 1, nightEnd.tm_hour, nightEnd.tm_min);
    #endif
    
    // Process hourly forecasts and categorize them
    for (int i = 0; i < hourlyForecastCount; i++) {
        // Get the date/time for this forecast
        time_t forecastTime = hourlyForecasts[i].dt;
        
        // Only process the next 48 hours
        if (forecastTime > now + 48*3600) {
            break;
        }
        
        // Check which period this forecast belongs to
        if (forecastTime >= morningStartTime && forecastTime < morningEndTime) {
            // Morning forecast
            morningTempSum += hourlyForecasts[i].temp;
            morningPopSum += hourlyForecasts[i].pop;
            
            // For morning forecast, ensure we use day icons
            String iconCode = hourlyForecasts[i].weather_icon;
            if (iconCode.length() >= 3 && iconCode.charAt(iconCode.length() - 1) == 'n') {
                iconCode = iconCode.substring(0, iconCode.length() - 1) + "d";
#if WEATHER_DEBUG
                Serial.printf("Converting morning icon %s to day icon %s\n", 
                             hourlyForecasts[i].weather_icon.c_str(), iconCode.c_str());
#endif
            }
            morningIcons.push_back(iconCode);
            morningCount++;
            
#if WEATHER_DEBUG
            struct tm* debugTimeinfo = localtime(&forecastTime);
            Serial.printf("Added to morning forecast: %02d.%02d %02d:%02d, icon: %s\n",
                         debugTimeinfo->tm_mday, debugTimeinfo->tm_mon + 1,
                         debugTimeinfo->tm_hour, debugTimeinfo->tm_min,
                         iconCode.c_str());
#endif
        }
        
        if (forecastTime >= afternoonStartTime && forecastTime < afternoonEndTime) {
            // Afternoon forecast
            afternoonTempSum += hourlyForecasts[i].temp;
            afternoonPopSum += hourlyForecasts[i].pop;
            
            // For afternoon forecast, always ensure we use day icons
            String iconCode = hourlyForecasts[i].weather_icon;
            if (iconCode.length() >= 3 && iconCode.charAt(iconCode.length() - 1) == 'n') {
                iconCode = iconCode.substring(0, iconCode.length() - 1) + "d";
#if WEATHER_DEBUG
                Serial.printf("Converting afternoon icon %s to day icon %s\n", 
                             hourlyForecasts[i].weather_icon.c_str(), iconCode.c_str());
#endif
            }
            afternoonIcons.push_back(iconCode);
            afternoonCount++;
            
#if WEATHER_DEBUG
            struct tm* debugTimeinfo = localtime(&forecastTime);
            Serial.printf("Added to afternoon forecast: %02d.%02d %02d:%02d, icon: %s\n",
                         debugTimeinfo->tm_mday, debugTimeinfo->tm_mon + 1,
                         debugTimeinfo->tm_hour, debugTimeinfo->tm_min,
                         iconCode.c_str());
#endif
        }
        
        if (forecastTime >= nightStartTime && forecastTime < nightEndTime) {
            // Night forecast
            nightTempSum += hourlyForecasts[i].temp;
            nightPopSum += hourlyForecasts[i].pop;
            
            // For night forecast, ensure we use night icons
            String iconCode = hourlyForecasts[i].weather_icon;
            if (iconCode.length() >= 3 && iconCode.charAt(iconCode.length() - 1) == 'd') {
                iconCode = iconCode.substring(0, iconCode.length() - 1) + "n";
#if WEATHER_DEBUG
                Serial.printf("Converting night icon %s to night icon %s\n", 
                             hourlyForecasts[i].weather_icon.c_str(), iconCode.c_str());
#endif
            }
            nightIcons.push_back(iconCode);
            nightCount++;
            
#if WEATHER_DEBUG
            struct tm* debugTimeinfo = localtime(&forecastTime);
            Serial.printf("Added to night forecast: %02d.%02d %02d:%02d, icon: %s\n",
                         debugTimeinfo->tm_mday, debugTimeinfo->tm_mon + 1,
                         debugTimeinfo->tm_hour, debugTimeinfo->tm_min,
                         iconCode.c_str());
#endif
        }
    }
    // Calculate morning forecast summary
    if (morningCount > 0) {
        morningForecast.avgTemp = morningTempSum / morningCount;
        morningForecast.avgPop = morningPopSum / morningCount;
        morningForecast.iconCode = getMostFrequentIcon(morningIcons);
        
        // We've already enforced day icons during collection, but double check here
        if (morningForecast.iconCode.length() >= 3 && 
            morningForecast.iconCode.charAt(morningForecast.iconCode.length() - 1) == 'n') {
            morningForecast.iconCode = 
                morningForecast.iconCode.substring(0, morningForecast.iconCode.length() - 1) + "d";
            
            #if WEATHER_DEBUG
            Serial.printf("Final correction: Morning icon set to %s\n", morningForecast.iconCode.c_str());
            #endif
        }
    } else {
        morningForecast.avgTemp = 0;
        morningForecast.avgPop = 0;
        morningForecast.iconCode = "03d"; // Default cloudy icon
        
        #if WEATHER_DEBUG
        Serial.println("No morning forecasts found, using default icon 03d");
        #endif
    }
    
    // Calculate afternoon forecast summary
    if (afternoonCount > 0) {
        afternoonForecast.avgTemp = afternoonTempSum / afternoonCount;
        afternoonForecast.avgPop = afternoonPopSum / afternoonCount;
        afternoonForecast.iconCode = getMostFrequentIcon(afternoonIcons);
        
        // We've already enforced day icons during collection, but double check here
        if (afternoonForecast.iconCode.length() >= 3 && 
            afternoonForecast.iconCode.charAt(afternoonForecast.iconCode.length() - 1) == 'n') {
            afternoonForecast.iconCode = 
                afternoonForecast.iconCode.substring(0, afternoonForecast.iconCode.length() - 1) + "d";
            
            #if WEATHER_DEBUG
            Serial.printf("Final correction: Afternoon icon set to %s\n", afternoonForecast.iconCode.c_str());
            #endif
        }
    } else {
        afternoonForecast.avgTemp = 0;
        afternoonForecast.avgPop = 0;
        afternoonForecast.iconCode = "03d"; // Default cloudy icon
        
        #if WEATHER_DEBUG
        Serial.println("No afternoon forecasts found, using default icon 03d");
        #endif
    }
    
    // Calculate night forecast summary
    if (nightCount > 0) {
        nightForecast.avgTemp = nightTempSum / nightCount;
        nightForecast.avgPop = nightPopSum / nightCount;
        nightForecast.iconCode = getMostFrequentIcon(nightIcons);
        
        // We've already enforced night icons during collection, but double check here
        if (nightForecast.iconCode.length() >= 3 && 
            nightForecast.iconCode.charAt(nightForecast.iconCode.length() - 1) == 'd') {
            nightForecast.iconCode = 
                nightForecast.iconCode.substring(0, nightForecast.iconCode.length() - 1) + "n";
            
            #if WEATHER_DEBUG
            Serial.printf("Final correction: Night icon set to %s\n", nightForecast.iconCode.c_str());
            #endif
        }
    } else {
        nightForecast.avgTemp = 0;
        nightForecast.avgPop = 0;
        nightForecast.iconCode = "03n"; // Default cloudy night icon
        
        #if WEATHER_DEBUG
        Serial.println("No night forecasts found, using default icon 03n");
        #endif
    }
    
    #if WEATHER_DEBUG
    Serial.println("Final forecast summary:");
    Serial.printf("  Morning: %.1f°C, %d%% pop, icon: %s (%d datapoints)\n", 
                 morningForecast.avgTemp, (int)(morningForecast.avgPop * 100), 
                 morningForecast.iconCode.c_str(), morningCount);
    Serial.printf("  Afternoon: %.1f°C, %d%% pop, icon: %s (%d datapoints)\n", 
                 afternoonForecast.avgTemp, (int)(afternoonForecast.avgPop * 100), 
                 afternoonForecast.iconCode.c_str(), afternoonCount);
    Serial.printf("  Night: %.1f°C, %d%% pop, icon: %s (%d datapoints)\n", 
                 nightForecast.avgTemp, (int)(nightForecast.avgPop * 100), 
                 nightForecast.iconCode.c_str(), nightCount);
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
const void* WeatherService::getIconForCode(const String& iconCode) {
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
