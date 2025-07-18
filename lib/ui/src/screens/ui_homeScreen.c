// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.5.2
// LVGL version: 9.2.2
// Project name: RadioweckerAI

#include "../ui.h"

lv_obj_t * uic_nightRainLabel;
lv_obj_t * uic_nightTempLabel;
lv_obj_t * uic_nightIcon;
lv_obj_t * uic_nightTitle;
lv_obj_t * uic_nightPanel;
lv_obj_t * uic_afternoonRainLabel;
lv_obj_t * uic_afternoonTempLabel;
lv_obj_t * uic_afternoonIcon;
lv_obj_t * uic_afternoonTitle;
lv_obj_t * uic_afternoonPanel;
lv_obj_t * uic_morningTempLabel;
lv_obj_t * uic_morningIcon;
lv_obj_t * uic_morningTitle;
lv_obj_t * uic_morningPanel;
lv_obj_t * uic_weatherDescLabel;
lv_obj_t * uic_feelsLikeLabel;
lv_obj_t * uic_currentTempLabel;
lv_obj_t * uic_ContCurrentWeather;
lv_obj_t * uic_weatherPanel;
lv_obj_t * uic_tvocLabel;
lv_obj_t * uic_ContTvocLabel;
lv_obj_t * uic_eco2Label;
lv_obj_t * uic_ContCo2Label;
lv_obj_t * uic_humidityLabel;
lv_obj_t * uic_ContHumLabel;
lv_obj_t * uic_tempLabel;
lv_obj_t * uic_ContTempLabel;
lv_obj_t * uic_ContSensorValues;
lv_obj_t * uic_tvocTitle;
lv_obj_t * uic_ContTvocTitle;
lv_obj_t * uic_co2Title;
lv_obj_t * uic_ContCo2Title;
lv_obj_t * uic_humTitle;
lv_obj_t * uic_ContHumTitle;
lv_obj_t * uic_tempLabel;
lv_obj_t * uic_ContTempTitle;
lv_obj_t * uic_ContSensorTitles;
lv_obj_t * uic_sensorPanel;
lv_obj_t * uic_NextAlarm;
lv_obj_t * uic_CurrentTime;
lv_obj_t * uic_CurrentDate;
lv_obj_t * uic_timePanel;
lv_obj_t * uic_wifiLabel;
lv_obj_t * uic_wifiQualityLabel;
lv_obj_t * uic_ipLabel;
lv_obj_t * uic_StatusBar;
lv_obj_t * uic_homeScreenPanel;
lv_obj_t * uic_homeScreen;
lv_obj_t * ui_homeScreen;
lv_obj_t * ui_homeScreenPanel;
lv_obj_t * ui_StatusBar;
lv_obj_t * ui_ipLabel;
lv_obj_t * ui_wifiQualityLabel;
lv_obj_t * ui_wifiLabel;
lv_obj_t * ui_timePanel;
lv_obj_t * ui_CurrentDate;
lv_obj_t * ui_CurrentTime;
lv_obj_t * ui_NextAlarm;
lv_obj_t * ui_sensorPanel;
lv_obj_t * ui_ContSensorTitles;
lv_obj_t * ui_ContTempTitle;
lv_obj_t * ui_tempTitle;
lv_obj_t * ui_ContHumTitle;
lv_obj_t * ui_humTitle;
lv_obj_t * ui_ContCo2Title;
lv_obj_t * ui_co2Title;
lv_obj_t * ui_ContTvocTitle;
lv_obj_t * ui_tvocTitle;
lv_obj_t * ui_ContSensorValues;
lv_obj_t * ui_ContTempLabel;
lv_obj_t * ui_tempLabel;
lv_obj_t * ui_ContHumLabel;
lv_obj_t * ui_humidityLabel;
lv_obj_t * ui_ContCo2Label;
lv_obj_t * ui_eco2Label;
lv_obj_t * ui_ContTvocLabel;
lv_obj_t * ui_tvocLabel;
lv_obj_t * ui_weatherPanel;
lv_obj_t * ui_ContCurrentWeather;
lv_obj_t * ui_currentWeatherPanel;
lv_obj_t * ui_currentWeatherTitle;
lv_obj_t * ui_Image1;
lv_obj_t * ui_currentTempLabel;
lv_obj_t * ui_feelsLikeLabel;
lv_obj_t * ui_weatherDescLabel;
lv_obj_t * ui_contForecast;
lv_obj_t * ui_morningPanel;
lv_obj_t * ui_morningTitle;
lv_obj_t * ui_morningIcon;
lv_obj_t * ui_morningTempLabel;
lv_obj_t * ui_morningRainLabel;
lv_obj_t * ui_afternoonPanel;
lv_obj_t * ui_afternoonTitle;
lv_obj_t * ui_afternoonIcon;
lv_obj_t * ui_afternoonTempLabel;
lv_obj_t * ui_afternoonRainLabel;
lv_obj_t * ui_nightPanel;
lv_obj_t * ui_nightTitle;
lv_obj_t * ui_nightIcon;
lv_obj_t * ui_nightTempLabel;
lv_obj_t * ui_nightRainLabel;
lv_obj_t * ui_Container1;

// event funtions
void ui_event_Container1(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_CLICKED) {
        _ui_screen_change(&ui_MenuScreen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 0, &ui_MenuScreen_screen_init);
    }
}

// build funtions

void ui_homeScreen_screen_init(void)
{
    ui_homeScreen = lv_obj_create(NULL);
    lv_obj_remove_flag(ui_homeScreen, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_homeScreen, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_homeScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_homeScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_homeScreen, lv_color_hex(0x797777), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_homeScreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_homeScreenPanel = lv_obj_create(ui_homeScreen);
    lv_obj_remove_style_all(ui_homeScreenPanel);
    lv_obj_set_width(ui_homeScreenPanel, lv_pct(100));
    lv_obj_set_height(ui_homeScreenPanel, lv_pct(100));
    lv_obj_set_align(ui_homeScreenPanel, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_homeScreenPanel, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_homeScreenPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_homeScreenPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_homeScreenPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_homeScreenPanel, lv_color_hex(0xB1AFAF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_homeScreenPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_StatusBar = lv_obj_create(ui_homeScreenPanel);
    lv_obj_set_height(ui_StatusBar, 35);
    lv_obj_set_width(ui_StatusBar, lv_pct(100));
    lv_obj_set_align(ui_StatusBar, LV_ALIGN_TOP_MID);
    lv_obj_remove_flag(ui_StatusBar, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_StatusBar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_StatusBar, lv_color_hex(0x4E4C4C), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_StatusBar, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_StatusBar, lv_color_hex(0xB1AFAF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_StatusBar, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_StatusBar, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_StatusBar, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_ipLabel = lv_label_create(ui_StatusBar);
    lv_obj_set_width(ui_ipLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_ipLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_ipLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_ipLabel, "IP: --");

    ui_wifiQualityLabel = lv_label_create(ui_StatusBar);
    lv_obj_set_width(ui_wifiQualityLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_wifiQualityLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_wifiQualityLabel, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_wifiQualityLabel, "-- %");

    ui_wifiLabel = lv_label_create(ui_StatusBar);
    lv_obj_set_width(ui_wifiLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_wifiLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_wifiLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_wifiLabel, "WiFi: --");

    ui_timePanel = lv_obj_create(ui_homeScreenPanel);
    lv_obj_set_height(ui_timePanel, 370);
    lv_obj_set_width(ui_timePanel, lv_pct(80));
    lv_obj_set_x(ui_timePanel, 0);
    lv_obj_set_y(ui_timePanel, 35);
    lv_obj_remove_flag(ui_timePanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_timePanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_timePanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_timePanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_timePanel, lv_color_hex(0xB1AFAF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_timePanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_timePanel, lv_color_hex(0x7B1010), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_timePanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_CurrentDate = lv_label_create(ui_timePanel);
    lv_obj_set_width(ui_CurrentDate, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_CurrentDate, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_CurrentDate, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_CurrentDate, "Donnerstag 31.03.2025");
    lv_obj_set_style_text_font(ui_CurrentDate, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_CurrentTime = lv_label_create(ui_timePanel);
    lv_obj_set_width(ui_CurrentTime, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_CurrentTime, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_CurrentTime, LV_ALIGN_CENTER);
    lv_label_set_text(ui_CurrentTime, "22:22:22");
    lv_obj_set_style_text_font(ui_CurrentTime, &ui_font_ms80m, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_NextAlarm = lv_label_create(ui_timePanel);
    lv_obj_set_width(ui_NextAlarm, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_NextAlarm, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_NextAlarm, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(ui_NextAlarm, "Nächster Alarm: Donnerstag 31.12.2025 22:22");
    lv_obj_set_style_text_font(ui_NextAlarm, &ui_font_ms16m, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_sensorPanel = lv_obj_create(ui_homeScreenPanel);
    lv_obj_set_height(ui_sensorPanel, 75);
    lv_obj_set_width(ui_sensorPanel, lv_pct(80));
    lv_obj_set_align(ui_sensorPanel, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_remove_flag(ui_sensorPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_sensorPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_sensorPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_sensorPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_sensorPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_sensorPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_sensorPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_sensorPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_sensorPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_sensorPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_ContSensorTitles = lv_obj_create(ui_sensorPanel);
    lv_obj_remove_style_all(ui_ContSensorTitles);
    lv_obj_set_width(ui_ContSensorTitles, lv_pct(100));
    lv_obj_set_height(ui_ContSensorTitles, lv_pct(50));
    lv_obj_set_align(ui_ContSensorTitles, LV_ALIGN_TOP_MID);
    lv_obj_remove_flag(ui_ContSensorTitles, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_text_font(ui_ContSensorTitles, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_ContTempTitle = lv_obj_create(ui_ContSensorTitles);
    lv_obj_remove_style_all(ui_ContTempTitle);
    lv_obj_set_width(ui_ContTempTitle, lv_pct(25));
    lv_obj_set_height(ui_ContTempTitle, lv_pct(100));
    lv_obj_set_align(ui_ContTempTitle, LV_ALIGN_LEFT_MID);
    lv_obj_remove_flag(ui_ContTempTitle, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_tempTitle = lv_label_create(ui_ContTempTitle);
    lv_obj_set_width(ui_tempTitle, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_tempTitle, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_tempTitle, LV_ALIGN_CENTER);
    lv_label_set_text(ui_tempTitle, "TEMPERATURE");

    ui_ContHumTitle = lv_obj_create(ui_ContSensorTitles);
    lv_obj_remove_style_all(ui_ContHumTitle);
    lv_obj_set_width(ui_ContHumTitle, lv_pct(25));
    lv_obj_set_height(ui_ContHumTitle, lv_pct(100));
    lv_obj_set_y(ui_ContHumTitle, 0);
    lv_obj_set_x(ui_ContHumTitle, lv_pct(25));
    lv_obj_set_align(ui_ContHumTitle, LV_ALIGN_LEFT_MID);
    lv_obj_remove_flag(ui_ContHumTitle, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_humTitle = lv_label_create(ui_ContHumTitle);
    lv_obj_set_width(ui_humTitle, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_humTitle, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_humTitle, LV_ALIGN_CENTER);
    lv_label_set_text(ui_humTitle, "HUMIDITY");

    ui_ContCo2Title = lv_obj_create(ui_ContSensorTitles);
    lv_obj_remove_style_all(ui_ContCo2Title);
    lv_obj_set_width(ui_ContCo2Title, lv_pct(25));
    lv_obj_set_height(ui_ContCo2Title, lv_pct(100));
    lv_obj_set_x(ui_ContCo2Title, lv_pct(50));
    lv_obj_set_y(ui_ContCo2Title, lv_pct(0));
    lv_obj_set_align(ui_ContCo2Title, LV_ALIGN_LEFT_MID);
    lv_obj_remove_flag(ui_ContCo2Title, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_co2Title = lv_label_create(ui_ContCo2Title);
    lv_obj_set_width(ui_co2Title, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_co2Title, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_co2Title, LV_ALIGN_CENTER);
    lv_label_set_text(ui_co2Title, "CO2");

    ui_ContTvocTitle = lv_obj_create(ui_ContSensorTitles);
    lv_obj_remove_style_all(ui_ContTvocTitle);
    lv_obj_set_width(ui_ContTvocTitle, lv_pct(25));
    lv_obj_set_height(ui_ContTvocTitle, lv_pct(100));
    lv_obj_set_x(ui_ContTvocTitle, lv_pct(75));
    lv_obj_set_y(ui_ContTvocTitle, lv_pct(0));
    lv_obj_set_align(ui_ContTvocTitle, LV_ALIGN_LEFT_MID);
    lv_obj_remove_flag(ui_ContTvocTitle, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_tvocTitle = lv_label_create(ui_ContTvocTitle);
    lv_obj_set_width(ui_tvocTitle, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_tvocTitle, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_tvocTitle, LV_ALIGN_CENTER);
    lv_label_set_text(ui_tvocTitle, "TVOC");

    ui_ContSensorValues = lv_obj_create(ui_sensorPanel);
    lv_obj_remove_style_all(ui_ContSensorValues);
    lv_obj_set_width(ui_ContSensorValues, lv_pct(100));
    lv_obj_set_height(ui_ContSensorValues, lv_pct(50));
    lv_obj_set_align(ui_ContSensorValues, LV_ALIGN_BOTTOM_MID);
    lv_obj_remove_flag(ui_ContSensorValues, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_text_font(ui_ContSensorValues, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_ContTempLabel = lv_obj_create(ui_ContSensorValues);
    lv_obj_remove_style_all(ui_ContTempLabel);
    lv_obj_set_width(ui_ContTempLabel, lv_pct(25));
    lv_obj_set_height(ui_ContTempLabel, lv_pct(100));
    lv_obj_set_align(ui_ContTempLabel, LV_ALIGN_LEFT_MID);
    lv_obj_remove_flag(ui_ContTempLabel, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_tempLabel = lv_label_create(ui_ContTempLabel);
    lv_obj_set_width(ui_tempLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_tempLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_tempLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_tempLabel, "--°C");

    ui_ContHumLabel = lv_obj_create(ui_ContSensorValues);
    lv_obj_remove_style_all(ui_ContHumLabel);
    lv_obj_set_width(ui_ContHumLabel, lv_pct(25));
    lv_obj_set_height(ui_ContHumLabel, lv_pct(100));
    lv_obj_set_y(ui_ContHumLabel, 0);
    lv_obj_set_x(ui_ContHumLabel, lv_pct(25));
    lv_obj_set_align(ui_ContHumLabel, LV_ALIGN_LEFT_MID);
    lv_obj_remove_flag(ui_ContHumLabel, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_humidityLabel = lv_label_create(ui_ContHumLabel);
    lv_obj_set_width(ui_humidityLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_humidityLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_humidityLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_humidityLabel, "--%");

    ui_ContCo2Label = lv_obj_create(ui_ContSensorValues);
    lv_obj_remove_style_all(ui_ContCo2Label);
    lv_obj_set_width(ui_ContCo2Label, lv_pct(25));
    lv_obj_set_height(ui_ContCo2Label, lv_pct(100));
    lv_obj_set_x(ui_ContCo2Label, lv_pct(50));
    lv_obj_set_y(ui_ContCo2Label, lv_pct(0));
    lv_obj_set_align(ui_ContCo2Label, LV_ALIGN_LEFT_MID);
    lv_obj_remove_flag(ui_ContCo2Label, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_eco2Label = lv_label_create(ui_ContCo2Label);
    lv_obj_set_width(ui_eco2Label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_eco2Label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_eco2Label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_eco2Label, "---");

    ui_ContTvocLabel = lv_obj_create(ui_ContSensorValues);
    lv_obj_remove_style_all(ui_ContTvocLabel);
    lv_obj_set_width(ui_ContTvocLabel, lv_pct(25));
    lv_obj_set_height(ui_ContTvocLabel, lv_pct(100));
    lv_obj_set_x(ui_ContTvocLabel, lv_pct(75));
    lv_obj_set_y(ui_ContTvocLabel, lv_pct(0));
    lv_obj_set_align(ui_ContTvocLabel, LV_ALIGN_LEFT_MID);
    lv_obj_remove_flag(ui_ContTvocLabel, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_tvocLabel = lv_label_create(ui_ContTvocLabel);
    lv_obj_set_width(ui_tvocLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_tvocLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_tvocLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_tvocLabel, "---");

    ui_weatherPanel = lv_obj_create(ui_homeScreenPanel);
    lv_obj_set_height(ui_weatherPanel, 445);
    lv_obj_set_width(ui_weatherPanel, lv_pct(20));
    lv_obj_set_x(ui_weatherPanel, 0);
    lv_obj_set_y(ui_weatherPanel, 35);
    lv_obj_set_align(ui_weatherPanel, LV_ALIGN_TOP_RIGHT);
    lv_obj_remove_flag(ui_weatherPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_weatherPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_weatherPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_weatherPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_weatherPanel, lv_color_hex(0xB1AFAF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_weatherPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_weatherPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_weatherPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_weatherPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_weatherPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_ContCurrentWeather = lv_obj_create(ui_weatherPanel);
    lv_obj_remove_style_all(ui_ContCurrentWeather);
    lv_obj_set_width(ui_ContCurrentWeather, lv_pct(100));
    lv_obj_set_height(ui_ContCurrentWeather, lv_pct(33));
    lv_obj_set_align(ui_ContCurrentWeather, LV_ALIGN_TOP_MID);
    lv_obj_remove_flag(ui_ContCurrentWeather, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_border_color(ui_ContCurrentWeather, lv_color_hex(0xB1AFAF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_ContCurrentWeather, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_currentWeatherPanel = lv_obj_create(ui_ContCurrentWeather);
    lv_obj_set_width(ui_currentWeatherPanel, lv_pct(100));
    lv_obj_set_height(ui_currentWeatherPanel, lv_pct(100));
    lv_obj_set_align(ui_currentWeatherPanel, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_currentWeatherPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_currentWeatherPanel, lv_color_hex(0xA29C9C), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_currentWeatherPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_currentWeatherPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_currentWeatherPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_currentWeatherPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_currentWeatherPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_currentWeatherPanel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_currentWeatherPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_currentWeatherPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_currentWeatherPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_currentWeatherTitle = lv_label_create(ui_currentWeatherPanel);
    lv_obj_set_width(ui_currentWeatherTitle, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_currentWeatherTitle, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_currentWeatherTitle, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_currentWeatherTitle, "Aktuelles Wetter");

    ui_Image1 = lv_image_create(ui_currentWeatherPanel);
    lv_image_set_src(ui_Image1, &ui_img_weather_icons_02d_png);
    lv_obj_set_width(ui_Image1, LV_SIZE_CONTENT);   /// 50
    lv_obj_set_height(ui_Image1, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_Image1, 0);
    lv_obj_set_y(ui_Image1, -30);
    lv_obj_set_align(ui_Image1, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Image1, LV_OBJ_FLAG_CLICKABLE);     /// Flags
    lv_obj_remove_flag(ui_Image1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_currentTempLabel = lv_label_create(ui_currentWeatherPanel);
    lv_obj_set_width(ui_currentTempLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_currentTempLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_currentTempLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_currentTempLabel, "--°C");
    lv_obj_set_style_text_font(ui_currentTempLabel, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_feelsLikeLabel = lv_label_create(ui_currentWeatherPanel);
    lv_obj_set_width(ui_feelsLikeLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_feelsLikeLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_feelsLikeLabel, 0);
    lv_obj_set_y(ui_feelsLikeLabel, 25);
    lv_obj_set_align(ui_feelsLikeLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_feelsLikeLabel, "Gefühlt: --°C");
    lv_obj_set_style_text_font(ui_feelsLikeLabel, &ui_font_ms14m, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_weatherDescLabel = lv_label_create(ui_currentWeatherPanel);
    lv_obj_set_width(ui_weatherDescLabel, lv_pct(90));
    lv_obj_set_height(ui_weatherDescLabel, lv_pct(25));
    lv_obj_set_align(ui_weatherDescLabel, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(ui_weatherDescLabel, "Keine Daten");
    lv_obj_set_style_text_align(ui_weatherDescLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_weatherDescLabel, &ui_font_ms14m, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_contForecast = lv_obj_create(ui_weatherPanel);
    lv_obj_remove_style_all(ui_contForecast);
    lv_obj_set_width(ui_contForecast, lv_pct(100));
    lv_obj_set_height(ui_contForecast, lv_pct(66));
    lv_obj_set_align(ui_contForecast, LV_ALIGN_BOTTOM_MID);
    lv_obj_remove_flag(ui_contForecast, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_morningPanel = lv_obj_create(ui_contForecast);
    lv_obj_set_width(ui_morningPanel, lv_pct(100));
    lv_obj_set_height(ui_morningPanel, lv_pct(33));
    lv_obj_set_align(ui_morningPanel, LV_ALIGN_TOP_MID);
    lv_obj_remove_flag(ui_morningPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_morningPanel, lv_color_hex(0x505050), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_morningPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_morningPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_morningPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_morningPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_morningPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_morningPanel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_morningPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_morningPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_morningPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_morningTitle = lv_label_create(ui_morningPanel);
    lv_obj_set_width(ui_morningTitle, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_morningTitle, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_morningTitle, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_morningTitle, "Vormittag");

    ui_morningIcon = lv_image_create(ui_morningPanel);
    lv_image_set_src(ui_morningIcon, &ui_img_weather_icons_02d_png);
    lv_obj_set_width(ui_morningIcon, LV_SIZE_CONTENT);   /// 50
    lv_obj_set_height(ui_morningIcon, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_morningIcon, 0);
    lv_obj_set_y(ui_morningIcon, 10);
    lv_obj_set_align(ui_morningIcon, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(ui_morningIcon, LV_OBJ_FLAG_CLICKABLE);     /// Flags
    lv_obj_remove_flag(ui_morningIcon, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_morningTempLabel = lv_label_create(ui_morningPanel);
    lv_obj_set_width(ui_morningTempLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_morningTempLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_morningTempLabel, 0);
    lv_obj_set_y(ui_morningTempLabel, -20);
    lv_obj_set_align(ui_morningTempLabel, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(ui_morningTempLabel, "--°C");

    ui_morningRainLabel = lv_label_create(ui_morningPanel);
    lv_obj_set_width(ui_morningRainLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_morningRainLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_morningRainLabel, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(ui_morningRainLabel, "Regen: --%");

    ui_afternoonPanel = lv_obj_create(ui_contForecast);
    lv_obj_set_width(ui_afternoonPanel, lv_pct(100));
    lv_obj_set_height(ui_afternoonPanel, lv_pct(33));
    lv_obj_set_x(ui_afternoonPanel, 0);
    lv_obj_set_y(ui_afternoonPanel, lv_pct(34));
    lv_obj_set_align(ui_afternoonPanel, LV_ALIGN_TOP_MID);
    lv_obj_remove_flag(ui_afternoonPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_afternoonPanel, lv_color_hex(0x505050), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_afternoonPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_afternoonPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_afternoonPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_afternoonPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_afternoonPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_afternoonPanel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_afternoonPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_afternoonPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_afternoonPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_afternoonTitle = lv_label_create(ui_afternoonPanel);
    lv_obj_set_width(ui_afternoonTitle, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_afternoonTitle, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_afternoonTitle, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_afternoonTitle, "Nachmittag");

    ui_afternoonIcon = lv_image_create(ui_afternoonPanel);
    lv_image_set_src(ui_afternoonIcon, &ui_img_weather_icons_02d_png);
    lv_obj_set_width(ui_afternoonIcon, LV_SIZE_CONTENT);   /// 50
    lv_obj_set_height(ui_afternoonIcon, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_afternoonIcon, 0);
    lv_obj_set_y(ui_afternoonIcon, 10);
    lv_obj_set_align(ui_afternoonIcon, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(ui_afternoonIcon, LV_OBJ_FLAG_CLICKABLE);     /// Flags
    lv_obj_remove_flag(ui_afternoonIcon, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_afternoonTempLabel = lv_label_create(ui_afternoonPanel);
    lv_obj_set_width(ui_afternoonTempLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_afternoonTempLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_afternoonTempLabel, 0);
    lv_obj_set_y(ui_afternoonTempLabel, -20);
    lv_obj_set_align(ui_afternoonTempLabel, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(ui_afternoonTempLabel, "--°C");

    ui_afternoonRainLabel = lv_label_create(ui_afternoonPanel);
    lv_obj_set_width(ui_afternoonRainLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_afternoonRainLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_afternoonRainLabel, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(ui_afternoonRainLabel, "Regen: --%");

    ui_nightPanel = lv_obj_create(ui_contForecast);
    lv_obj_set_width(ui_nightPanel, lv_pct(100));
    lv_obj_set_height(ui_nightPanel, lv_pct(32));
    lv_obj_set_x(ui_nightPanel, 0);
    lv_obj_set_y(ui_nightPanel, lv_pct(68));
    lv_obj_set_align(ui_nightPanel, LV_ALIGN_TOP_MID);
    lv_obj_remove_flag(ui_nightPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_nightPanel, lv_color_hex(0x505050), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_nightPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_nightPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_nightPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_nightPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_nightPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_nightPanel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_nightPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_nightPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_nightPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_nightTitle = lv_label_create(ui_nightPanel);
    lv_obj_set_width(ui_nightTitle, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_nightTitle, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_nightTitle, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_nightTitle, "Nacht");

    ui_nightIcon = lv_image_create(ui_nightPanel);
    lv_image_set_src(ui_nightIcon, &ui_img_weather_icons_02d_png);
    lv_obj_set_width(ui_nightIcon, LV_SIZE_CONTENT);   /// 50
    lv_obj_set_height(ui_nightIcon, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_nightIcon, 0);
    lv_obj_set_y(ui_nightIcon, 10);
    lv_obj_set_align(ui_nightIcon, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(ui_nightIcon, LV_OBJ_FLAG_CLICKABLE);     /// Flags
    lv_obj_remove_flag(ui_nightIcon, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_nightTempLabel = lv_label_create(ui_nightPanel);
    lv_obj_set_width(ui_nightTempLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_nightTempLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_nightTempLabel, 0);
    lv_obj_set_y(ui_nightTempLabel, -20);
    lv_obj_set_align(ui_nightTempLabel, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(ui_nightTempLabel, "--°C");

    ui_nightRainLabel = lv_label_create(ui_nightPanel);
    lv_obj_set_width(ui_nightRainLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_nightRainLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_nightRainLabel, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(ui_nightRainLabel, "Regen: --%");

    ui_Container1 = lv_obj_create(ui_homeScreen);
    lv_obj_remove_style_all(ui_Container1);
    lv_obj_set_width(ui_Container1, lv_pct(100));
    lv_obj_set_height(ui_Container1, lv_pct(100));
    lv_obj_set_align(ui_Container1, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_Container1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    lv_obj_add_event_cb(ui_Container1, ui_event_Container1, LV_EVENT_ALL, NULL);
    uic_homeScreen = ui_homeScreen;
    uic_homeScreenPanel = ui_homeScreenPanel;
    uic_StatusBar = ui_StatusBar;
    uic_ipLabel = ui_ipLabel;
    uic_wifiQualityLabel = ui_wifiQualityLabel;
    uic_wifiLabel = ui_wifiLabel;
    uic_timePanel = ui_timePanel;
    uic_CurrentDate = ui_CurrentDate;
    uic_CurrentTime = ui_CurrentTime;
    uic_NextAlarm = ui_NextAlarm;
    uic_sensorPanel = ui_sensorPanel;
    uic_ContSensorTitles = ui_ContSensorTitles;
    uic_ContTempTitle = ui_ContTempTitle;
    uic_tempLabel = ui_tempTitle;
    uic_ContHumTitle = ui_ContHumTitle;
    uic_humTitle = ui_humTitle;
    uic_ContCo2Title = ui_ContCo2Title;
    uic_co2Title = ui_co2Title;
    uic_ContTvocTitle = ui_ContTvocTitle;
    uic_tvocTitle = ui_tvocTitle;
    uic_ContSensorValues = ui_ContSensorValues;
    uic_ContTempLabel = ui_ContTempLabel;
    uic_tempLabel = ui_tempLabel;
    uic_ContHumLabel = ui_ContHumLabel;
    uic_humidityLabel = ui_humidityLabel;
    uic_ContCo2Label = ui_ContCo2Label;
    uic_eco2Label = ui_eco2Label;
    uic_ContTvocLabel = ui_ContTvocLabel;
    uic_tvocLabel = ui_tvocLabel;
    uic_weatherPanel = ui_weatherPanel;
    uic_ContCurrentWeather = ui_ContCurrentWeather;
    uic_currentTempLabel = ui_currentTempLabel;
    uic_feelsLikeLabel = ui_feelsLikeLabel;
    uic_weatherDescLabel = ui_weatherDescLabel;
    uic_morningPanel = ui_morningPanel;
    uic_morningTitle = ui_morningTitle;
    uic_morningIcon = ui_morningIcon;
    uic_morningTempLabel = ui_morningTempLabel;
    uic_afternoonPanel = ui_afternoonPanel;
    uic_afternoonTitle = ui_afternoonTitle;
    uic_afternoonIcon = ui_afternoonIcon;
    uic_afternoonTempLabel = ui_afternoonTempLabel;
    uic_afternoonRainLabel = ui_afternoonRainLabel;
    uic_nightPanel = ui_nightPanel;
    uic_nightTitle = ui_nightTitle;
    uic_nightIcon = ui_nightIcon;
    uic_nightTempLabel = ui_nightTempLabel;
    uic_nightRainLabel = ui_nightRainLabel;

}

void ui_homeScreen_screen_destroy(void)
{
    if(ui_homeScreen) lv_obj_del(ui_homeScreen);

    // NULL screen variables
    uic_homeScreen = NULL;
    ui_homeScreen = NULL;
    uic_homeScreenPanel = NULL;
    ui_homeScreenPanel = NULL;
    uic_StatusBar = NULL;
    ui_StatusBar = NULL;
    uic_ipLabel = NULL;
    ui_ipLabel = NULL;
    uic_wifiQualityLabel = NULL;
    ui_wifiQualityLabel = NULL;
    uic_wifiLabel = NULL;
    ui_wifiLabel = NULL;
    uic_timePanel = NULL;
    ui_timePanel = NULL;
    uic_CurrentDate = NULL;
    ui_CurrentDate = NULL;
    uic_CurrentTime = NULL;
    ui_CurrentTime = NULL;
    uic_NextAlarm = NULL;
    ui_NextAlarm = NULL;
    uic_sensorPanel = NULL;
    ui_sensorPanel = NULL;
    uic_ContSensorTitles = NULL;
    ui_ContSensorTitles = NULL;
    uic_ContTempTitle = NULL;
    ui_ContTempTitle = NULL;
    uic_tempLabel = NULL;
    ui_tempTitle = NULL;
    uic_ContHumTitle = NULL;
    ui_ContHumTitle = NULL;
    uic_humTitle = NULL;
    ui_humTitle = NULL;
    uic_ContCo2Title = NULL;
    ui_ContCo2Title = NULL;
    uic_co2Title = NULL;
    ui_co2Title = NULL;
    uic_ContTvocTitle = NULL;
    ui_ContTvocTitle = NULL;
    uic_tvocTitle = NULL;
    ui_tvocTitle = NULL;
    uic_ContSensorValues = NULL;
    ui_ContSensorValues = NULL;
    uic_ContTempLabel = NULL;
    ui_ContTempLabel = NULL;
    uic_tempLabel = NULL;
    ui_tempLabel = NULL;
    uic_ContHumLabel = NULL;
    ui_ContHumLabel = NULL;
    uic_humidityLabel = NULL;
    ui_humidityLabel = NULL;
    uic_ContCo2Label = NULL;
    ui_ContCo2Label = NULL;
    uic_eco2Label = NULL;
    ui_eco2Label = NULL;
    uic_ContTvocLabel = NULL;
    ui_ContTvocLabel = NULL;
    uic_tvocLabel = NULL;
    ui_tvocLabel = NULL;
    uic_weatherPanel = NULL;
    ui_weatherPanel = NULL;
    uic_ContCurrentWeather = NULL;
    ui_ContCurrentWeather = NULL;
    ui_currentWeatherPanel = NULL;
    ui_currentWeatherTitle = NULL;
    ui_Image1 = NULL;
    uic_currentTempLabel = NULL;
    ui_currentTempLabel = NULL;
    uic_feelsLikeLabel = NULL;
    ui_feelsLikeLabel = NULL;
    uic_weatherDescLabel = NULL;
    ui_weatherDescLabel = NULL;
    ui_contForecast = NULL;
    uic_morningPanel = NULL;
    ui_morningPanel = NULL;
    uic_morningTitle = NULL;
    ui_morningTitle = NULL;
    uic_morningIcon = NULL;
    ui_morningIcon = NULL;
    uic_morningTempLabel = NULL;
    ui_morningTempLabel = NULL;
    ui_morningRainLabel = NULL;
    uic_afternoonPanel = NULL;
    ui_afternoonPanel = NULL;
    uic_afternoonTitle = NULL;
    ui_afternoonTitle = NULL;
    uic_afternoonIcon = NULL;
    ui_afternoonIcon = NULL;
    uic_afternoonTempLabel = NULL;
    ui_afternoonTempLabel = NULL;
    uic_afternoonRainLabel = NULL;
    ui_afternoonRainLabel = NULL;
    uic_nightPanel = NULL;
    ui_nightPanel = NULL;
    uic_nightTitle = NULL;
    ui_nightTitle = NULL;
    uic_nightIcon = NULL;
    ui_nightIcon = NULL;
    uic_nightTempLabel = NULL;
    ui_nightTempLabel = NULL;
    uic_nightRainLabel = NULL;
    ui_nightRainLabel = NULL;
    ui_Container1 = NULL;

}
