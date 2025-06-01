/**
 * @file lv_conf.h
 * Configuration file for LVGL v9.2.2
 */

#ifndef LV_CONF_H
#define LV_CONF_H

/*====================
 * COLOR SETTINGS
 *====================*/

/*Color depth: 1 (1 byte per pixel), 8 (RGB332), 16 (RGB565), 32 (ARGB8888)*/
#define LV_COLOR_DEPTH 16

/*====================
 * MEMORY SETTINGS
 *====================*/

/*Size of the memory available for 'lv_malloc()' in bytes (>= 2kB)*/
#define LV_MEM_SIZE               (49 * 1024U)      /*[bytes]*/
#define LV_MEM_CUSTOM             1

/*====================
 * HAL SETTINGS
 *====================*/

/*Use a custom tick source that tells the elapsed time in milliseconds.
 *It removes the need to manually update the tick with `lv_tick_inc()`)*/
#define LV_TICK_CUSTOM            1
#if LV_TICK_CUSTOM
    #define LV_TICK_CUSTOM_INCLUDE  <Arduino.h>         /*Header for the system time function*/
    #define LV_TICK_CUSTOM_SYS_TIME_EXPR  (millis())    /*Expression evaluating to current system time in ms*/
#endif   /*LV_TICK_CUSTOM*/

/*=======================
 * FEATURE CONFIGURATION
 *=======================*/

/*-------------
 * Logging
 *-----------*/
#define LV_USE_LOG               1
#if LV_USE_LOG

    /*How important log should be added:
    *LV_LOG_LEVEL_TRACE       A lot of logs to give detailed information
    *LV_LOG_LEVEL_INFO        Log important events
    *LV_LOG_LEVEL_WARN        Log if something unwanted happened but didn't cause a problem
    *LV_LOG_LEVEL_ERROR       Only critical issue, when the system may fail
    *LV_LOG_LEVEL_USER        Only logs added by the user
    *LV_LOG_LEVEL_NONE        Do not log anything*/
    #define LV_LOG_LEVEL       LV_LOG_LEVEL_WARN

    /*1: Print the log with 'printf';
    *0: User need to register a callback with `lv_log_register_print_cb()`*/
    #define LV_LOG_PRINTF      1
#endif  /*LV_USE_LOG*/

/*-------------
 * Themes
 *-----------*/
/*A simple, impressive and very complete theme*/
#define LV_USE_THEME_DEFAULT      1
#if LV_USE_THEME_DEFAULT
    /*0: Light mode; 1: Dark mode*/
    #define LV_THEME_DEFAULT_DARK 0
#endif

/*Enable the animations*/
#define LV_USE_ANIMATION          1

/*1: Show CPU usage and FPS count*/
#define LV_USE_PERF_MONITOR       0
#if LV_USE_PERF_MONITOR
    #define LV_USE_PERF_MONITOR_POS LV_ALIGN_BOTTOM_RIGHT
#endif

/*1: Show the used memory and the memory fragmentation
 * Requires LV_MEM_CUSTOM = 0*/
#define LV_USE_MEM_MONITOR        0
#if LV_USE_MEM_MONITOR
    #define LV_USE_MEM_MONITOR_POS LV_ALIGN_BOTTOM_LEFT
#endif

/*==================
 * FONT USAGE
 *===================*/

/*Enable the built-in fonts*/
#define LV_FONT_MONTSERRAT_8      0
#define LV_FONT_MONTSERRAT_10     0
#define LV_FONT_MONTSERRAT_12     1  /*Used in sensor title labels*/
#define LV_FONT_MONTSERRAT_14     0
#define LV_FONT_MONTSERRAT_16     1  /*Used in some UI elements*/
#define LV_FONT_MONTSERRAT_18     0
#define LV_FONT_MONTSERRAT_20     1  /*Used in sensor value display*/
#define LV_FONT_MONTSERRAT_22     0
#define LV_FONT_MONTSERRAT_24     0
#define LV_FONT_MONTSERRAT_26     0
#define LV_FONT_MONTSERRAT_28     1  /*Used in current temperature display*/
#define LV_FONT_MONTSERRAT_30     0
#define LV_FONT_MONTSERRAT_32     1  /*Used in date display*/
#define LV_FONT_MONTSERRAT_34     0
#define LV_FONT_MONTSERRAT_36     0
#define LV_FONT_MONTSERRAT_38     0
#define LV_FONT_MONTSERRAT_40     0
#define LV_FONT_MONTSERRAT_42     0
#define LV_FONT_MONTSERRAT_44     0
#define LV_FONT_MONTSERRAT_46     0
#define LV_FONT_MONTSERRAT_48     0

/* Default font */
#define LV_FONT_DEFAULT          &lv_font_montserrat_12

/* Enable drawing placeholders when glyph dsc is not found */
#define LV_USE_FONT_PLACEHOLDER   1

/* Character encoding */
#define LV_TXT_ENC LV_TXT_ENC_UTF8

/*==================
 * USER DATA
 *==================*/

/* Data of object */
#define LV_USE_USER_DATA          1

#endif /*LV_CONF_H*/
