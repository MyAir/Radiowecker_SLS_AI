#ifndef LV_CONF_H
#define LV_CONF_H

/* Enable debug if level is high */
#ifndef LV_LOG_LEVEL
    #ifdef CORE_DEBUG_LEVEL
        #define LV_LOG_LEVEL CORE_DEBUG_LEVEL
    #else
        #define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
    #endif
#elif defined(LV_LOG_LEVEL)
    /* LV_LOG_LEVEL is already defined, keep it as is */
#endif

/* Memory settings */
#ifndef LV_MEM_SIZE
    #define LV_MEM_SIZE (48U * 1024U)  /*[bytes]*/
#endif

/* Default display color depth */
#ifndef LV_COLOR_DEPTH
    #define LV_COLOR_DEPTH 16
#endif

/* Enable only the fonts you need */
#ifndef LV_FONT_MONTSERRAT_8
    #define LV_FONT_MONTSERRAT_8  1
#endif
#ifndef LV_FONT_MONTSERRAT_10
    #define LV_FONT_MONTSERRAT_10 1
#endif
#ifndef LV_FONT_MONTSERRAT_12
    #define LV_FONT_MONTSERRAT_12 1
#endif
#ifndef LV_FONT_MONTSERRAT_14
    #define LV_FONT_MONTSERRAT_14 1
#endif
#ifndef LV_FONT_MONTSERRAT_16
    #define LV_FONT_MONTSERRAT_16 1
#endif
#ifndef LV_FONT_MONTSERRAT_18
    #define LV_FONT_MONTSERRAT_18 1
#endif
#ifndef LV_FONT_MONTSERRAT_20
    #define LV_FONT_MONTSERRAT_20 1
#endif
#ifndef LV_FONT_MONTSERRAT_22
    #define LV_FONT_MONTSERRAT_22 1
#endif
#ifndef LV_FONT_MONTSERRAT_24
    #define LV_FONT_MONTSERRAT_24 1
#endif

/* Default font */
#ifndef LV_FONT_DEFAULT
    #define LV_FONT_DEFAULT &lv_font_montserrat_14
#endif

/* Enable font features */
#ifndef LV_USE_FONT_PLACEHOLDER
    #define LV_USE_FONT_PLACEHOLDER 1
#endif

/* Enable extra features - these are controlled by platformio.ini */
#ifndef LV_USE_PERF_MONITOR
    #define LV_USE_PERF_MONITOR 0
#endif

#ifndef LV_USE_MEM_MONITOR
    #define LV_USE_MEM_MONITOR 0
#endif

/* Enable the modules you need */
#ifndef LV_USE_LOG
    #define LV_USE_LOG 1
#endif

#ifndef LV_USE_THEME_DEFAULT
    #define LV_USE_THEME_DEFAULT 1
#endif

/* Use custom tick source */
#ifndef LV_TICK_CUSTOM
    #define LV_TICK_CUSTOM 1
    #if LV_TICK_CUSTOM
        #ifndef LV_TICK_CUSTOM_INCLUDE
            #define LV_TICK_CUSTOM_INCLUDE <Arduino.h>
        #endif
        #ifndef LV_TICK_CUSTOM_SYS_TIME_EXPR
            #define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())
        #endif
    #endif
#endif

#endif /*LV_CONF_H*/
