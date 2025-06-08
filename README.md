

# Arduino Project MaTouch 4.3inch

## Prepare Arduino Library

Copy SquareLine_Project\libraries\lv_conf.h into the SquareLine_Project\libraries\lvgl\src folder.

Copy the libraries into C:\Users\YOUR_USER_NAME\Documents\Arduino\libraries.

If these libraries already exist, delete them. And the ui folder must be deleted before copying.


## Compile the Arduino program

Open the .ino file in Arduino.

Please select the compilation options for MaTouch according to the Makerfabs wiki or github.

## Update the modified UI

The SquareLine UI code is now compiled in Arduino as a library.

Each time you select "Export UI Files", SquareLine will update the ui library in the SquareLine_Project\libraries\ui\ of the project by default.

You need to replace the ui library in C:\Users\YOUR_USER_NAME\Documents\Arduino\libraries.

Or in Project Setting, redirect the UI export location, such as:

C:\Users\YOUR_USER_NAME\Documents\Arduino\libraries\ui\src


``` 

Use version 8.3.11 of the library lvgl 
Use version 1.3.1 of the library GFX_Library_for_Arduino 
Use version 2.0.0 library SPI 
Use version 1.0 of the library ui 
Use version 2.0.0 version of library Wire 
Use version 1.0.2 of the library TAMC_GT911 
Use version 6.21.3 of the library ArduinoJson

```

## Weather Service

The project includes a weather service that fetches data from OpenWeatherMap API. To use this feature:

1. Create a free account at [OpenWeatherMap](https://openweathermap.org/) and get an API key
2. Add the weather configuration to your `config.json` file on the SD card as shown below

```json
{
  "wifi": {
    "ssid": "YourWiFiNetwork",
    "password": "YourWiFiPassword"
  },
  "ntp": {
    "server1": "pool.ntp.org",
    "server2": "time.nist.gov",
    "timezone": "CET-1CEST,M3.5.0,M10.5.0/3"
  },
  "weather": {
    "apiKey": "YourOpenWeatherMapAPIKey",
    "latitude": 52.520008,
    "longitude": 13.404954,
    "units": "metric",
    "language": "de"
  }
}
```

The weather data is refreshed every 5 minutes and includes:
- Current temperature and conditions
- Morning forecast (6:00-12:00)
- Afternoon forecast (12:00-18:00)
- Night forecast (18:00-24:00)