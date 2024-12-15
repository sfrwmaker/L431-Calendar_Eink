/*
 * epd2in7b.h
 *
 */

#ifndef E_INK_EPD27B_H_
#define E_INK_EPD27B_H_

#include "common.h"
#include "u8g_font.h"

#ifdef __cplusplus
extern "C" {
#endif

bool 	EPD27B_Init(uint8_t layers, bool full);
void 	EPD27B_DisplayPartial(uint16_t x, uint16_t y, uint16_t w, uint16_t l, bool fast_lut, bool mono);
void	EPD27B_Display(bool black_on_top, bool fast_lut, bool mono, bool wait);
void	EPD27B_Sleep(void);
void 	EPD27B_InitDspl(void);


#ifdef __cplusplus
}
#endif

#endif /* E_INK_EPD27B_H_ */
