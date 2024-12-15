/*
 * epd75.h
 *
 *  Created on: Feb 16, 2024
 *      Author: Alex
 */

#ifndef E_INK_EPD75_H_
#define E_INK_EPD75_H_

#include "common.h"
#include "u8g_font.h"

#ifdef __cplusplus
extern "C" {
#endif

bool	EPD75_Init(uint8_t layers, bool full);
void	EPD75_InitDsplBW(void);
void	EPD75_InitDsplRBW(void);
void 	EPD75_ActivatePartialMode(void);
void	EPD75_InitDspFast(void);
void	EPD75_Display(void);
void 	EPD75_DisplayPartial(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
bool	EPD75_NextPage(void);

#ifdef __cplusplus
}
#endif

#endif
