/*
 * epd2in7b.cpp
 *
 */

#include <epd27b.h>
#include "main.h"

// By default the display is in portrait mode
#define EPD27B_SCREEN_WIDTH 	(176)
#define EPD27B_SCREEN_HEIGHT 	(264)

const GPIO_PinState EPD27B_BUSY 			= GPIO_PIN_RESET;

// epd2in7b commands
// Full update color lut tables
static const uint8_t lut_vcom_dc[44] = {
		0x00, 0x00,
		0x00, 0x1A, 0x1A, 0x00, 0x00, 0x01,
		0x00, 0x0A, 0x0A, 0x00, 0x00, 0x08,
		0x00, 0x0E, 0x01, 0x0E, 0x01, 0x10,
		0x00, 0x0A, 0x0A, 0x00, 0x00, 0x08,
		0x00, 0x04, 0x10, 0x00, 0x00, 0x05,
		0x00, 0x03, 0x0E, 0x00, 0x00, 0x0A,
		0x00, 0x23, 0x00, 0x00, 0x00, 0x01
};
static const uint8_t lut_ww[42] = {							//R21H
		0x90, 0x1A, 0x1A, 0x00, 0x00, 0x01,
		0x40, 0x0A, 0x0A, 0x00, 0x00, 0x08,
		0x84, 0x0E, 0x01, 0x0E, 0x01, 0x10,
		0x80, 0x0A, 0x0A, 0x00, 0x00, 0x08,
		0x00, 0x04, 0x10, 0x00, 0x00, 0x05,
		0x00, 0x03, 0x0E, 0x00, 0x00, 0x0A,
		0x00, 0x23, 0x00, 0x00, 0x00, 0x01
};
static const uint8_t lut_bw[42] = {							//R22H    r
		0xA0, 0x1A, 0x1A, 0x00, 0x00, 0x01,
		0x00, 0x0A, 0x0A, 0x00, 0x00, 0x08,
		0x84, 0x0E, 0x01, 0x0E, 0x01, 0x10,
		0x90, 0x0A, 0x0A, 0x00, 0x00, 0x08,
		0xB0, 0x04, 0x10, 0x00, 0x00, 0x05,
		0xB0, 0x03, 0x0E, 0x00, 0x00, 0x0A,
		0xC0, 0x23, 0x00, 0x00, 0x00, 0x01
};
static const uint8_t lut_bb[42] = {							//R23H    w
		0x90, 0x1A, 0x1A, 0x00, 0x00, 0x01,
		0x40, 0x0A, 0x0A, 0x00, 0x00, 0x08,
		0x84, 0x0E, 0x01, 0x0E, 0x01, 0x10,
		0x80, 0x0A, 0x0A, 0x00, 0x00, 0x08,
		0x00, 0x04, 0x10, 0x00, 0x00, 0x05,
		0x00, 0x03, 0x0E, 0x00, 0x00, 0x0A,
		0x00, 0x23, 0x00, 0x00, 0x00, 0x01
};
static const uint8_t lut_wb[42] = {							//R24H    b
		0x90, 0x1A, 0x1A, 0x00, 0x00, 0x01,
		0x20, 0x0A, 0x0A, 0x00, 0x00, 0x08,
		0x84, 0x0E, 0x01, 0x0E, 0x01, 0x10,
		0x10, 0x0A, 0x0A, 0x00, 0x00, 0x08,
		0x00, 0x04, 0x10, 0x00, 0x00, 0x05,
		0x00, 0x03, 0x0E, 0x00, 0x00, 0x0A,
		0x00, 0x23, 0x00, 0x00, 0x00, 0x01
};
// Full update Black and White only lut tables
static const uint8_t lut_mono_vcom_dc[44] = {
		0x00, 0x00,
		0x00, 0x08, 0x00, 0x00, 0x00, 0x02,
		0x60, 0x28, 0x28, 0x00, 0x00, 0x01,
		0x00, 0x14, 0x00, 0x00, 0x00, 0x01,
		0x00, 0x12, 0x12, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const uint8_t lut_mono_ww[42] = {
		0x40, 0x08, 0x00, 0x00, 0x00, 0x02,
		0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
		0x40, 0x14, 0x00, 0x00, 0x00, 0x01,
		0xA0, 0x12, 0x12, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const uint8_t lut_mono_bw[42] = {
		0x40, 0x08, 0x00, 0x00, 0x00, 0x02,
		0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
		0x40, 0x14, 0x00, 0x00, 0x00, 0x01,
		0xA0, 0x12, 0x12, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const uint8_t lut_mono_bb[42] = {
		0x80, 0x08, 0x00, 0x00, 0x00, 0x02,
		0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
		0x80, 0x14, 0x00, 0x00, 0x00, 0x01,
		0x50, 0x12, 0x12, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const uint8_t lut_mono_wb[42] = {
		0x80, 0x08, 0x00, 0x00, 0x00, 0x02,
		0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
		0x80, 0x14, 0x00, 0x00, 0x00, 0x01,
		0x50, 0x12, 0x12, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Local function forward declarations
static void		EPD27B_SetLut(void);
static void		EPD27B_SetMonoLut(void);
static void		EPD27B_SetFastLut(void);
static void 	EPD27B_SetFastLutTable(const uint8_t lut_table[]);

bool EPD27B_Init(uint8_t layers, bool full) {
	EPD27B_InitDspl();
	EPD_SendCommand(PARTIAL_DISPLAY_REFRESH);
	EPD_SendData(0x00);

	return EPD_Init(EPD27B_SCREEN_WIDTH, EPD27B_SCREEN_HEIGHT, layers, full);
}

void EPD27B_InitDspl(void) {
	EPD_Reset();

	EPD_SendCommand(POWER_SETTING);
	EPD_SendData(0x03);                  					// VDS_EN, VDG_EN
	EPD_SendData(0x00);                  					// VCOM_HV, VGHL_LV[1], VGHL_LV[0]
	EPD_SendData(0x2b);                  					// VDH
	EPD_SendData(0x2b);                  					// VDL
	EPD_SendData(0x09);                  					// VDHR

	EPD_SendCommand(BOOSTER_SOFT_START);
	EPD_SendData(0x07);
	EPD_SendData(0x07);
	EPD_SendData(0x17);

	EPD_SendCommand(POWER_OPTIMIZATION);
	EPD_SendData(0x60);
	EPD_SendData(0xA5);

	EPD_SendCommand(POWER_OPTIMIZATION);
	EPD_SendData(0x89);
	EPD_SendData(0xA5);

	EPD_SendCommand(POWER_OPTIMIZATION);
	EPD_SendData(0x90);
	EPD_SendData(0x00);

	EPD_SendCommand(POWER_OPTIMIZATION);
	EPD_SendData(0x93);
	EPD_SendData(0x2A);

	EPD_SendCommand(POWER_OPTIMIZATION);
	EPD_SendData(0x73);
	EPD_SendData(0x41);

	EPD_SendCommand(VCM_DC_SETTING_REGISTER);
	EPD_SendData(0x12);
	EPD_SendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
	EPD_SendData(0x87);        								// define by OTP

	EPD27B_SetLut();
}

void EPD27B_DisplayPartial(uint16_t x, uint16_t y, uint16_t w, uint16_t l, bool fast_lut, bool mono) {
	// Translate rectangle coordinates to absolute value (is display rotated)
	Rectangle r;
	r.x_upl		= x;
	r.y_upl		= y;
	r.x_lor		= x + w;
	r.y_lor		= y + l;
	if (!EPD_AbsRectangle(&r)) return;						// Invalid coordinates
	x = r.x_upl;
	y = r.y_upl;
	w = r.x_lor - x;										// Columns number (horizontal pixel)
	l = r.y_lor - y;										// Rows number (lines)

	// x should be the multiple of 8
	w += x & 7;												// left border could be moved left, increase area width to cover initial rectangle
	x &= 0xf8;												// Now x is multiple of 8
	w = (w+7) & 0xf8;										// Sometimes add extra byte right to cover initial rectangle
	if (x + w > EPD27B_SCREEN_WIDTH) {						// Prevent out of screen
		w = EPD27B_SCREEN_WIDTH - x;
	}

	EPD_SendCommand(POWER_ON);
	EPD_WaitDisplayReady(EPD27B_BUSY);

	EPD_SendCommand(PANEL_SETTING);
	EPD_SendData(0xaf);        									//KW-BF   KWR-AF    BWROTP 0f

	EPD_SendCommand(PLL_CONTROL);
	EPD_SendData(0x3a);											// 3A 100HZ   29 150Hz 39 200HZ    31 171HZ

	uint8_t  bytes_per_row = (EPD27B_SCREEN_WIDTH + 7) / 8;
	uint16_t first_byte = y * bytes_per_row + (x >> 3);

	if (mono) {
		uint8_t *layer_data = EPD_GetLayerData(MODE_BLACK);
		if (layer_data) {
			EPD_SendCommand(PARTIAL_DATA_START_TRANSMISSION_2);
			EPD_SetPartialArea(x, y, w, l);
			HAL_Delay(2);
			for (uint16_t i = 0; i < l; ++i) {				// Through rows
			  	for (uint16_t j = 0; j < w / 8; ++j) {		// Through line
			  		EPD_SendData(~layer_data[first_byte + j]);
			   	}
			  	first_byte += bytes_per_row;
			}
			HAL_Delay(2);
		}
		EPD27B_SetMonoLut();
	} else {												// Two color mode
		for (uint8_t i = 0; i < 2; ++i) {					// Actually supports 2 layers only
			uint8_t *layer_data = EPD_GetLayerData((i==1)?MODE_RED:MODE_BLACK);
			if (layer_data) {
				EPD_SendCommand((i==1)?PARTIAL_DATA_START_TRANSMISSION_2:PARTIAL_DATA_START_TRANSMISSION_1);
				EPD_SetPartialArea(x, y, w, l);
				HAL_Delay(2);
				for (uint16_t i = 0; i < l; ++i) {				// Through rows
					for (uint16_t j = 0; j < w / 8; ++j) {		// Through line
						EPD_SendData(layer_data[first_byte + j]);
					}
					first_byte += bytes_per_row;
				}
				HAL_Delay(2);
			}
		}
		if (fast_lut)
			EPD27B_SetFastLut();
		else
			EPD27B_SetLut();
	}

	EPD_SendCommand(PARTIAL_DISPLAY_REFRESH);
	EPD_SetPartialArea(x, y, w, l);

	EPD_WaitDisplayReady(EPD27B_BUSY);
}

void EPD27B_Display(bool black_on_top, bool fast_lut, bool mono, bool wait) {
  	uint16_t width	= EPD27B_SCREEN_WIDTH;
  	uint16_t height = EPD27B_SCREEN_HEIGHT;

  	if (black_on_top) {
  		uint8_t *b = EPD_GetLayerData(MODE_BLACK);
  		uint8_t *r = EPD_GetLayerData(MODE_RED);
  		for (uint16_t i = 0; i < height*width/8; ++i) {
  			r[i] &= ~b[i];									// Clear bits on red layer that is active of black layer
  		}
  	}

  	EPD_SendCommand(POWER_ON);
	EPD_WaitDisplayReady(EPD27B_BUSY);

	EPD_SendCommand(PANEL_SETTING);
	EPD_SendData(0xaf);        								// KW-BF   KWR-AF    BWROTP 0f

	EPD_SendCommand(PLL_CONTROL);
	EPD_SendData(0x3a);       								// 3A 100HZ   29 150Hz 39 200HZ    31 171HZ

	EPD_SendCommand(TCON_RESOLUTION);
	EPD_SendData(width >> 8);
	EPD_SendData(width & 0xff);
	EPD_SendData(height >> 8);
	EPD_SendData(height & 0xff);

  	uint16_t data_size = height * width/8;
  	uint8_t *data = EPD_GetLayerData(MODE_BLACK);
  	if (mono) {
  		EPD_SendCommand(DATA_START_TRANSMISSION_2);
  		HAL_Delay(2);
  		for (uint16_t i = 0; i < data_size; ++i)
  			EPD_SendData(~data[i]);
  		HAL_Delay(2);
  		EPD27B_SetMonoLut();
  	} else {
  		EPD_SendCommand(DATA_START_TRANSMISSION_1);
  		HAL_Delay(2);
  		for (uint16_t i = 0; i < data_size; ++i)
  			EPD_SendData(data[i]);
  		HAL_Delay(2);

  		data = EPD_GetLayerData(MODE_RED);
  		EPD_SendCommand(DATA_START_TRANSMISSION_2);
  		HAL_Delay(2);
  		for (uint16_t i = 0; i < data_size; ++i)
  			EPD_SendData(data[i]);
  		HAL_Delay(2);

  	  	if (fast_lut)
  	  		EPD27B_SetFastLut();
  	  	else
  	  		EPD27B_SetLut();
  	}

  	EPD_SendCommand(DISPLAY_REFRESH);
  	if (wait)
  		EPD_WaitDisplayReady(EPD27B_BUSY);

}

static void EPD27B_SetLut(void) {
	EPD_SendCommand(LUT_FOR_VCOM);							//vcom
	for (uint8_t i = 0; i < 44; ++i)
		EPD_SendData(lut_vcom_dc[i]);
	EPD_SendCommand(LUT_WHITE_TO_WHITE);					//ww --
	for (uint8_t i = 0; i < 42; ++i)
		EPD_SendData(lut_ww[i]);
	EPD_SendCommand(LUT_BLACK_TO_WHITE);					//bw r
	for (uint8_t i = 0; i < 42; ++i)
		EPD_SendData(lut_bw[i]);
	EPD_SendCommand(LUT_WHITE_TO_BLACK);					//wb w
	for (uint8_t i = 0; i < 42; ++i)
		EPD_SendData(lut_bb[i]);
	EPD_SendCommand(LUT_BLACK_TO_BLACK);					//bb b
	for (uint8_t i = 0; i < 42; ++i)
		EPD_SendData(lut_wb[i]);
}

static void EPD27B_SetMonoLut(void) {
	EPD_SendCommand(LUT_FOR_VCOM);							//vcom
	for (uint8_t i = 0; i < 44; ++i)
		EPD_SendData(lut_mono_vcom_dc[i]);
	EPD_SendCommand(LUT_WHITE_TO_WHITE);					//ww --
	for (uint8_t i = 0; i < 42; ++i)
		EPD_SendData(lut_mono_ww[i]);
	EPD_SendCommand(LUT_BLACK_TO_WHITE);					//bw r
	for (uint8_t i = 0; i < 42; ++i)
		EPD_SendData(lut_mono_bw[i]);
	EPD_SendCommand(LUT_WHITE_TO_BLACK);					//wb w
	for (uint8_t i = 0; i < 42; ++i)
		EPD_SendData(lut_mono_bb[i]);
	EPD_SendCommand(LUT_BLACK_TO_BLACK);					//bb b
	for (uint8_t i = 0; i < 42; ++i)
		EPD_SendData(lut_mono_wb[i]);
}

static void EPD27B_SetFastLut(void) {
	EPD_SendCommand(LUT_FOR_VCOM);							//vcom
	EPD_SendData(lut_vcom_dc[0]);
	EPD_SendData(lut_vcom_dc[1]);
	EPD27B_SetFastLutTable(&lut_vcom_dc[2]);

	EPD_SendCommand(LUT_WHITE_TO_WHITE);					//ww --
	EPD27B_SetFastLutTable(lut_ww);
	EPD_SendCommand(LUT_BLACK_TO_WHITE);					//bw r
	EPD27B_SetFastLutTable(lut_bw);
	EPD_SendCommand(LUT_WHITE_TO_BLACK);					//wb w
	EPD27B_SetFastLutTable(lut_bb);
	EPD_SendCommand(LUT_BLACK_TO_BLACK);					//bb b
	EPD27B_SetFastLutTable(lut_wb);
}

static void EPD27B_SetFastLutTable(const uint8_t lut_table[]) {
	static const uint8_t lut_fast_repeats[7] = {
			0x01, 0x02, 0x04, 0x02, 0x03, 0x04, 0x01
	};
	uint8_t lut[42];
	for (uint8_t i = 0; i < 42; ++i)						// First, copy origital table
		lut[i] = lut_table[i];
	for (uint8_t i = 0; i < 7; ++i)							// Replace repeat counter
		lut[i*6+5]  = lut_fast_repeats[i];
	for (uint8_t i = 0; i < 42; ++i)
		EPD_SendData(lut[i]);
}
