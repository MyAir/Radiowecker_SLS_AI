#pragma once

// This centralized header file controls all debug flags
// It can be included in any source file that needs debug flags

// Check if UI_DEBUG is on - if so, enable all debug outputs
#if defined(UI_DEBUG) && UI_DEBUG == 1
  #ifndef STATUS_DEBUG
    #define STATUS_DEBUG 1
  #endif
  #ifndef TIME_DEBUG
    #define TIME_DEBUG 1
  #endif
  #ifndef SENSOR_DEBUG
    #define SENSOR_DEBUG 1
  #endif
#endif

// Default values if not already defined
#ifndef STATUS_DEBUG
  #define STATUS_DEBUG 0
#endif

#ifndef TIME_DEBUG
  #define TIME_DEBUG 0
#endif

#ifndef SENSOR_DEBUG
  #define SENSOR_DEBUG 0
#endif
