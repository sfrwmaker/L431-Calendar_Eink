/*
 * epd4in2.h
 *
 *  Created on: 2024 FEB 14
 *      Author: Alex
 */

#ifndef E_INK_EPD42_H_
#define E_INK_EPD42_H_

#include "common.h"
#include "u8g_font.h"

#ifdef __cplusplus
extern "C" {
#endif

bool	EPD42_Init(uint8_t layers, bool full);
void 	EPD42_InitDspl(void);
void	EPD42_InitDsplPartial(void);
void	EPD42_Display(void);
void	EPD42_DisplayPartial(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

#ifdef __cplusplus
}
#endif

#endif
