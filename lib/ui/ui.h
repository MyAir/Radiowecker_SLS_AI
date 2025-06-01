/**
 * @file ui.h
 *
 */

#ifndef UI_OUTER_H
#define UI_OUTER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "src/ui.h"

/*********************
 *   FONT DECLARATIONS
 *********************/
#if LV_FONT_MONTSERRAT_12
LV_FONT_DECLARE(lv_font_montserrat_12);
#endif

#if LV_FONT_MONTSERRAT_16
LV_FONT_DECLARE(lv_font_montserrat_16);
#endif

#if LV_FONT_MONTSERRAT_20
LV_FONT_DECLARE(lv_font_montserrat_20);
#endif

#if LV_FONT_MONTSERRAT_28
LV_FONT_DECLARE(lv_font_montserrat_28);
#endif

#if LV_FONT_MONTSERRAT_32
LV_FONT_DECLARE(lv_font_montserrat_32);
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_OUTER_H*/

