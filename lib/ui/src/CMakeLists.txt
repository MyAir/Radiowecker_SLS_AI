SET(SOURCES screens/ui_homeScreen.c
    screens/ui_MenuScreen.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    images/ui_img_weather_icons_02d_png.c
    images/ui_img_weather_icons_01d_png.c
    images/ui_img_weather_icons_01n_png.c
    images/ui_img_weather_icons_02n_png.c
    images/ui_img_weather_icons_03d_png.c
    images/ui_img_weather_icons_03n_png.c
    images/ui_img_weather_icons_04d_png.c
    images/ui_img_weather_icons_04n_png.c
    images/ui_img_weather_icons_09d_png.c
    images/ui_img_weather_icons_09n_png.c
    images/ui_img_weather_icons_10d_png.c
    images/ui_img_weather_icons_10n_png.c
    images/ui_img_weather_icons_11d_png.c
    images/ui_img_weather_icons_11n_png.c
    images/ui_img_weather_icons_13d_png.c
    images/ui_img_weather_icons_13n_png.c
    images/ui_img_weather_icons_50d_png.c
    images/ui_img_weather_icons_50n_png.c
    fonts/ui_font_ms14m.c
    fonts/ui_font_ms80m.c
    fonts/ui_font_ms16m.c)

add_library(ui ${SOURCES})
