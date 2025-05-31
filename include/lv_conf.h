#ifndef LV_CONF_H
#define LV_CONF_H

/* Enable debug if level is high */
#ifndef LV_LOG_LEVEL
    #ifdef CORE_DEBUG_LEVEL
        #define LV_LOG_LEVEL CORE_DEBUG_LEVEL
    #else
        #define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
    #endif
#endif

/* Memory settings */
#define LV_MEM_SIZE (48U * 1024U)  /*[bytes]*/

/* Default display color depth */
#define LV_COLOR_DEPTH 16

/* Enable only the fonts you need */
#define LV_FONT_MONTSERRAT_8  1
#define LV_FONT_MONTSERRAT_10 1
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_22 1
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_26 1
#define LV_FONT_MONTSERRAT_28 1
#define LV_FONT_MONTSERRAT_30 1
#define LV_FONT_MONTSERRAT_32 1

/* Default font */
#define LV_FONT_DEFAULT &lv_font_montserrat_14

/* Enable font features */
#define LV_USE_FONT_PLACEHOLDER 1

/* Enable extra features */
#define LV_USE_PERF_MONITOR 1
#define LV_USE_MEM_MONITOR 1

/* Enable the modules you need */
#define LV_USE_LOG 1
#define LV_USE_THEME_DEFAULT 1

/* Use custom tick source */
#define LV_TICK_CUSTOM 1
#if LV_TICK_CUSTOM
    #define LV_TICK_CUSTOM_INCLUDE <Arduino.h>
    #define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())
#endif

#endif /*LV_CONF_H*/
