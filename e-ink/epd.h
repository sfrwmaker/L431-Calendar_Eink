/*
 * epd.h
 *
 * 2024 SEP 15
 * 		The high-resolution displays require huge amount of controller memory to be allocated for the frame buffer. For instance,
 * 		the 7.5" display with resolution 800x480 requires the memory buffer of 48000 bytes in case on Black-White display and 96000 bytes
 * 		in case of Red-Black-White display.
 *  	To address the problem this driver implemented single memory buffer for multicolor displays.
 *  	The EPD_Init() routine or EPD::init() method allow to chose the way the driver allocates the memory buffer. If the display has
 *  	multiple colors, say R-B-W, the memory allocation depends on the last parameter, 'full'. If this parameter is true,
 *  	the driver tries to allocate separate memory buffer for each color (full-memory-buffer mode), otherwise, the single-memory-buffer
 *  	mode will be activated: the controller allocates the memory buffer for one layer only. The drawing procedure in this case will be
 *  	managed by loop through each color layer. The below you can see examples of how to use driver in different modes.
 *  	Full-memory-buffer mode (the memory allocated for all colors):
 *  		dspl.init();
 *  		dspl.drawPixel() or dsol.drawLine() or whatever procedure
 *  		dspl.display() - to write-out the frame buffer to the display
 *  	Single-memory-buffer mode (the memory allocated for one layer only):
 *  		dspl.init();
 *  		dspl.firstPage();
 *  		do {
 *				dspl.drawPixel() or dsol.drawLine() or whatever procedure
 *			} while (dspl.nextPage());
 *			The firstPage() returns false if the display is not in single-memory-buffer mode.
 *			The partial display refresh mode uses black layer only.
 *
 */

#ifndef E_INK_EPD_H_
#define E_INK_EPD_H_

#include "common.h"
#include "epd27b.h"
#include "epd42.h"
#include "epd75.h"
#include "u8g_font.h"

#ifdef __cplusplus
class EPD : public u8gFont {
	public:
		EPD()		: u8gFont()								{ }
		virtual			~EPD()								{ }
		virtual	void	init(uint8_t layers, bool full)		{ }
		virtual void	activatePartialMode(void)			{ }
		virtual void	initDspFast(void)					{ }
		void			sleepIn(void)						{ EPD_Sleep();														}
		virtual void 	sleepOut(void)						{ }
		virtual void	display(void)						{ }
		virtual void 	displayPartial(uint16_t x, uint16_t y, uint16_t w, uint16_t h)	{ }
		virtual void	displayColor(bool black_on_top, bool fast_lut) { }
		virtual bool	firstPage(void)						{ return EPD_FirstPage();											}
		virtual bool	nextPage(void)						{ return false;														}
		uint16_t		width(void)							{ return EPD_Width();												}
		uint16_t		height(void)						{ return EPD_Height();												}
		uint8_t			layers(void)						{ return EPD_GetLayerNumber();										}
		void			clear(COLOR c)						{ EPD_Clear(c);														}
		void 			drawPixel(uint16_t x, uint16_t y, COLOR c)
															{ EPD_DrawPixel(x, y, c);											}
		void 			drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, COLOR c)
															{ EPD_DrawLine(x0, y0, x1, y1, c);									}
		void 			drawHLine(uint16_t x, uint16_t y, uint16_t line_width, COLOR c)
															{ EPD_DrawHLine(x, y, line_width, c);								}
		void 			drawVLine(uint16_t x, uint16_t y, uint16_t line_height, COLOR c)
															{ EPD_DrawVLine(x, y, line_height, c);								}
		void 			drawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, COLOR c)
															{ EPD_DrawRectangle(x, y, w, h,c);									}
		void 			drawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, COLOR c)
															{ EPD_DrawFilledRectangle(x, y, w, h, c);							}
		void			drawDrawRoundRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, COLOR c)
															{ EPD_DrawRoundRect(x, y, w, h, r, c);								}
		void			drawFilledRoundRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, COLOR c)
															{ EPD_DrawFilledRoundRect(x, y, w, h, r, c);						}
		void 			drawCircle(uint16_t x, uint16_t y, uint8_t radius, COLOR c)
															{ EPD_DrawCircle(x, y, radius,c);									}
		void 			drawFilledCircle(uint16_t x, uint16_t y, uint8_t radius, COLOR c)
															{ EPD_DrawFilledCircle(x, y, radius, c);							}
		void			drawBitmap(uint16_t x, uint16_t y, const uint8_t *bm, uint16_t bm_width, uint16_t bm_height, COLOR fg, COLOR bg)
															{ EPD_DrawBitmap(x, y, bm, bm_width, bm_height, fg, bg); }
		void			setRotation(Rotation_t r)			{ EPD_SetRotation(r);												}
		void 			drawThickLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t thickness, COLOR color)
															{ EPD_DrawThickLine(x0, y0, x1, y1, thickness, color);				}
		void			drawVarThickLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, LineThickness thickness, COLOR color)
															{ EPD_DrawVarThickLine(x0, y0, x1, y1, thickness,color);			}

};

class EPD27B: public EPD {
	public:
		EPD27B()	: EPD()									{ }
		virtual	void	init(uint8_t layers, bool full)		{ EPD27B_Init(layers, full);										}
		virtual void	sleepOut(void)						{ EPD27B_InitDspl();												}
		virtual void	display(void)						{ EPD27B_Display(true, false, true, true);							}
		virtual void 	displayPartial(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
															{  EPD27B_DisplayPartial(x, y, w, h, false, false);					}
		virtual void 	displayPartial(uint16_t x, uint16_t y, uint16_t w, uint16_t l, bool fast_lut = false, bool mono = false)
															{ EPD27B_DisplayPartial(x, y, w, l, fast_lut, mono);				}
		virtual void	displayColor(bool black_on_top, bool fast_lut)
															{ EPD27B_Display(black_on_top, fast_lut, false, false);				}
};

class EPD75: public EPD {
	public:
		EPD75()	: EPD()										{ }
		virtual	void	init(uint8_t layers, bool full)		{ EPD75_Init(layers, full);											}
		virtual void	activatePartialMode(void)			{ EPD75_ActivatePartialMode();										}
		virtual void	initDspFast(void)					{ EPD75_InitDspFast();												}
		virtual void	sleepOut(void)						{ if (EPD_GetLayerNumber() > 1) EPD75_InitDsplRBW(); else EPD75_InitDsplBW();		 }
		virtual void	display(void)						{ EPD75_Display();													}
		virtual void 	displayPartial(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
															{ EPD75_DisplayPartial(x, y, w, h);									}
		virtual bool	nextPage(void)						{ return EPD75_NextPage();											}
		void			hwPower(bool on)					{ HAL_GPIO_WritePin(EPD_PWR_GPIO_Port, EPD_PWR_Pin, on?GPIO_PIN_SET:GPIO_PIN_RESET); }
};

#endif

#endif /* E_INK_EPD_H_ */
