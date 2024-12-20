/*
 * epd75.c
 *
 *  Created on: 2024 FEB 16
 *  	Based on EPD_7in5_V2_old.c and EPD_7in5b_V2.c from Waveshare
 *
 *  2024 AUG 23
 *  	Fixed x-coordinate and width issue in EPD75_DisplayPartial()
 *  2024 SEP 13
 *  	Implemented 3-color driver (R-B-W) through single memory buffer
 */

#include "epd75.h"
#include "main.h"

// By default the display is in portrait mode
#define EPD75_SCREEN_WIDTH 		(800)
#define EPD75_SCREEN_HEIGHT 	(480)

static const LUT lut_casual = {
		42, 42, 42, 42, 42,
		{
			0x0,	0xF,	0xF,	0x0,	0x0,	0x1,
			0x0,	0xF,	0x1,	0xF,	0x1,	0x2,
			0x0,	0xF,	0xF,	0x0,	0x0,	0x1,
			0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
			0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
			0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
			0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		// WW
			0x10,	0xF,	0xF,	0x0,	0x0,	0x1,
			0x84,	0xF,	0x1,	0xF,	0x1,	0x2,
			0x20,	0xF,	0xF,	0x0,	0x0,	0x1,
			0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
			0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
			0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
			0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		// BW
			0x10,	0xF,	0xF,	0x0,	0x0,	0x1,
			0x84,	0xF,	0x1,	0xF,	0x1,	0x2,
			0x20,	0xF,	0xF,	0x0,	0x0,	0x1,
			0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
			0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
			0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
			0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		// WB
			0x80,	0xF,	0xF,	0x0,	0x0,	0x1,
			0x84,	0xF,	0x1,	0xF,	0x1,	0x2,
			0x40,	0xF,	0xF,	0x0,	0x0,	0x1,
			0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
			0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
			0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
			0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		// BB
			0x80,	0xF,	0xF,	0x0,	0x0,	0x1,
			0x84,	0xF,	0x1,	0xF,	0x1,	0x2,
			0x40,	0xF,	0xF,	0x0,	0x0,	0x1,
			0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
			0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
			0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
			0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		}
};

static const LUT lut_partial = {
		42, 42, 42, 42, 42,
		{
			0x00,	0x14,	0x02,	0x00,	0x00,	0x01,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
		// WW
			0x20,	0x14,	0x02,	0x00,	0x00,	0x01,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
		// BW,
			0x80,	0x14,	0x02,	0x00,	0x00,	0x01,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
		// WB
			0x40,	0x14,	0x02,	0x00,	0x00,	0x01,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
		// BB
			0x00,	0x14,	0x02,	0x00,	0x00,	0x01,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00
		}
};

static const LUT lut_fast = {
		42, 42, 42, 42, 42,
		{
			0x00,	0x32,	0x32,	0x00,	0x00,	0x01,
			0x00,	0x0A,	0x0A,	0x00,	0x00,	0x00,
			0x00,	0x28,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
		// WW
			0x60,	0x32,	0x32,	0x00,	0x00,	0x01,
			0x60,	0x0A,	0x0A,	0x00,	0x00,	0x00,
			0x80,	0x28,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
		// BW
			0x60,	0x32,	0x32,	0x00,	0x00,	0x01,
			0x60,	0x0A,	0x0A,	0x00,	0x00,	0x00,
			0x80,	0x28,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
		// WB
			0x90,	0x32,	0x32,	0x00,	0x00,	0x01,
			0x60,   0x0A,	0x0A,	0x00,	0x00,	0x00,
			0x40,	0x28,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
		// BB
			0x90,	0x32,	0x32,	0x00,	0x00,	0x01,
			0x60,	0x0A,	0x0A,	0x00,	0x00,	0x00,
			0x40,	0x28,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
			0x00,	0x00,	0x00,	0x00,	0x00,	0x00
		}
};

// Local function forward declarations
static void 	EPD75_Init2(void);
static void		EPD75_WaitDisplayReady(void);

bool EPD75_Init(uint8_t layers, bool full) {
	if (layers == 1)
		EPD75_InitDsplBW();
	else
		EPD75_InitDsplRBW();
	EPD_SendCommand(PARTIAL_DISPLAY_REFRESH);
	EPD_SendData(0x00);

	return EPD_Init(EPD75_SCREEN_WIDTH, EPD75_SCREEN_HEIGHT, layers, full);
}

void EPD75_InitDsplBW(void) {
	EPD_Reset();

	EPD_SendCommand(POWER_SETTING);							// power setting
	EPD_SendData(0x17);  									// 1-0=11: internal power
	EPD_SendData(0x17);										// VGH&VGL
	EPD_SendData(0x3F);										// VSH
	EPD_SendData(0x3F);										// VSL
	EPD_SendData(0x11);										// VSHR

	EPD_SendCommand(VCM_DC_SETTING_REGISTER);				// VCOM DC Setting
	EPD_SendData(0x24);										// VCOM

	EPD_SendCommand(BOOSTER_SOFT_START);  					// Booster Setting
	EPD_SendData(0x27);
	EPD_SendData(0x27);
	EPD_SendData(0x2F);
	EPD_SendData(0x17);

	EPD_SendCommand(PLL_CONTROL);							// OSC Setting
	EPD_SendData(0x6);										// 2-0=100: N=4  ; 5-3=111: M=7  ;  3C=50Hz     3A=100HZ

	EPD_SendCommand(POWER_ON);
	EPD75_WaitDisplayReady();

	EPD_SendCommand(PANEL_SETTING);							// PANNEL SETTING
	EPD_SendData(0x3F);										// KW-3f   KWR-2F	BWROTP 0f	BWOTP 1f

	EPD_SendCommand(TCON_RESOLUTION);						// tres
	EPD_SendData(EPD75_SCREEN_WIDTH >> 8);
	EPD_SendData(EPD75_SCREEN_WIDTH & 0xff);
	EPD_SendData(EPD75_SCREEN_HEIGHT >> 8);
	EPD_SendData(EPD75_SCREEN_HEIGHT & 0xff);

	EPD_SendCommand(PARTIAL_DATA_START_TRANSMISSION_2);
	EPD_SendData(0x00);

	EPD_SendCommand(VCOM_AND_DATA_INTERVAL_SETTING);		// VCOM AND DATA INTERVAL SETTING
	EPD_SendData(0x10);
	EPD_SendData(0x00);

	EPD_SendCommand(TCON_SETTING);
	EPD_SendData(0x22);

	EPD_SendCommand(GATE_SOURCE_GSST);						// Resolution setting
	EPD_SendData(0x00);
	EPD_SendData(0x00);										// 800*480
	EPD_SendData(0x00);
	EPD_SendData(0x00);

	EPD_SetLut(&lut_casual);
}

void EPD75_InitDsplRBW(void) {
	EPD_Reset();

	EPD_SendCommand(POWER_SETTING);							// power setting
	EPD_SendData(0x07);
	EPD_SendData(0x07);    									// VGH=20V,VGL=-20V
	EPD_SendData(0x3f);										// VDH=15V
	EPD_SendData(0x3f);										// VDL=-15V

	EPD_SendCommand(POWER_ON); 								// POWER ON
	EPD75_WaitDisplayReady();

	EPD_SendCommand(PANEL_SETTING);							// PANNEL SETTING
	EPD_SendData(0x0F);   									// KW-3f   KWR-2F	BWROTP 0f	BWOTP 1f

	EPD_SendCommand(TCON_RESOLUTION);						// tres
	EPD_SendData(EPD75_SCREEN_WIDTH >> 8);
	EPD_SendData(EPD75_SCREEN_WIDTH & 0xff);
	EPD_SendData(EPD75_SCREEN_HEIGHT >> 8);
	EPD_SendData(EPD75_SCREEN_HEIGHT & 0xff);

	EPD_SendCommand(PARTIAL_DATA_START_TRANSMISSION_2);
	EPD_SendData(0x00);

	EPD_SendCommand(VCOM_AND_DATA_INTERVAL_SETTING);		// VCOM AND DATA INTERVAL SETTING
	EPD_SendData(0x11);
	EPD_SendData(0x07);

	EPD_SendCommand(TCON_SETTING);							// TCON SETTING
	EPD_SendData(0x22);

	EPD_SendCommand(VCM_DC_SETTING_REGISTER);				// VCOM DC Setting
	EPD_SendData(0x08);
	EPD_SendCommand(PLL_CONTROL);							// OSC Setting
	EPD_SendData(0x06);

	EPD_SendCommand(GATE_SOURCE_GSST);  					// Resolution setting
    EPD_SendData(0x00);
    EPD_SendData(0x00);										// 800*480
    EPD_SendData(0x00);
    EPD_SendData(0x00);
}


void EPD75_ActivatePartialMode(void) {
	EPD75_Init2();
	EPD_SetLut(&lut_partial);
}

void EPD75_InitDspFast(void) {
	EPD75_Init2();
	EPD_SetLut(&lut_fast);
}

void EPD75_Display(void) {
  	uint32_t data_size = (EPD75_SCREEN_HEIGHT * EPD75_SCREEN_WIDTH) >> 3;
  	uint8_t *data = EPD_GetLayerData(MODE_BLACK);

  	EPD_SendCommand(DATA_START_TRANSMISSION_1);
  	for (uint16_t i = 0; i < data_size; ++i)
  		EPD_SendData(~data[i]);

  	if (EPD_GetLayerNumber() > 1 && EPD_IsFullMemory())
  		data = EPD_GetLayerData(MODE_RED);
	EPD_SendCommand(DATA_START_TRANSMISSION_2);
	EPD_SendDataBuff(data, data_size);

	EPD_SendCommand(DISPLAY_REFRESH);
  	EPD75_WaitDisplayReady();
}

void EPD75_DisplayPartial(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
	// Translate rectangle coordinates to absolute value (is display rotated)
	Rectangle r;
	r.x_upl		= x;
	r.y_upl		= y;
	r.x_lor		= x + w;
	r.y_lor		= y + h;
	if (!EPD_AbsRectangle(&r)) return;						// Invalid coordinates
	x = r.x_upl;
	y = r.y_upl;
	w = r.x_lor - x;										// Columns number (horizontal pixel)
	h = r.y_lor - y;										// Rows number (lines)

	// x should be aligned to byte border
	w += x & 7;												// left border could be moved left, increase area width to cover initial rectangle
	x &= 0xfff8;											// Now x is at the byte border
	w = (w+7) & 0xfff8;										// Sometimes add extra byte right to cover initial rectangle
	if (x + w > EPD75_SCREEN_WIDTH) {						// Prevent out of screen
		w = EPD75_SCREEN_WIDTH - x;
	}

	uint8_t  bytes_per_row = (EPD75_SCREEN_WIDTH + 7) >> 3;
	uint16_t first_byte = y * bytes_per_row + (x >> 3);

	uint8_t *layer_data = EPD_GetLayerData(MODE_BLACK);
	if (layer_data) {
		EPD_SetPartialRectangle(x, y, x+w-1, y+h-1);

		if (EPD_GetLayerNumber() > 1) {						// R-B-W display
			EPD_SendCommand(DATA_START_TRANSMISSION_1);
			uint32_t area_size = (w*h) >> 3;				// The area size in bytes
			for (uint32_t i = 0; i < area_size; ++i) {
				EPD_SendData(0xff);
			}
		}
		EPD_SendCommand(DATA_START_TRANSMISSION_2);
		for (uint16_t i = 0; i < h; ++i) {					// Through rows
			for (uint16_t j = 0; j < w / 8; ++j) {			// Through line
				EPD_SendData(layer_data[first_byte + j]);
			}
			first_byte += bytes_per_row;
		}
		HAL_Delay(2);

		EPD_SendCommand(DISPLAY_REFRESH);
		EPD75_WaitDisplayReady();
		EPD_SendCommand(PARTIAL_PTOUT);
	}
}

bool EPD75_NextPage(void) {
  	uint32_t data_size = (EPD75_SCREEN_HEIGHT * EPD75_SCREEN_WIDTH) >> 3;
  	uint8_t *data = EPD_GetLayerData(MODE_BLACK);

  	uint8_t active_layer = EPD_GetActiveLayer();
  	switch (active_layer) {
  		case 1:												// Red
  			EPD_SendCommand(DATA_START_TRANSMISSION_2);
  			EPD_SendDataBuff(data, data_size);
  			break;
  		case 0:												// Black
  		default:
  			EPD_SendCommand(DATA_START_TRANSMISSION_1);
  			for (uint16_t i = 0; i < data_size; ++i)
  				EPD_SendData(~data[i]);
  			break;
  	}
  	if (EPD_TurnPage()) {
  		EPD_SendCommand(DISPLAY_REFRESH);
  		EPD75_WaitDisplayReady();
  		return false;										// Finish
  	}
  	return true;											// Ready for next page
}

static void EPD75_Init2(void) {
	static const uint8_t wavedata[] = {0x67, 0xBF, 0x3F, 0x0D, 0x00, 0x1C};

	EPD_Reset();

    EPD_SendCommand(PANEL_SETTING);
    EPD_SendData(0x3F);

    EPD_SendCommand(BOOSTER_SOFT_START);
    EPD_SendData(0x17);
    EPD_SendData(0x17);
    EPD_SendData(0x28);
    EPD_SendData(0x18);

    EPD_SendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
    EPD_SendData(0x22);
    EPD_SendData(0x07);

    EPD_SendCommand(TCON_SETTING);
    EPD_SendData(0x22);										// S-G G-S

    EPD_SendCommand(TCON_RESOLUTION);
    EPD_SendData(EPD75_SCREEN_WIDTH >> 8);
	EPD_SendData(EPD75_SCREEN_WIDTH & 0xff);
	EPD_SendData(EPD75_SCREEN_HEIGHT >> 8);
	EPD_SendData(EPD75_SCREEN_HEIGHT & 0xff);

    EPD_SendCommand(0x65);  // Resolution setting
    EPD_SendData(0x00);
	EPD_SendData(0x00);
	EPD_SendData(0x00);
	EPD_SendData(0x00);

    EPD_SendCommand(POWER_ON);
    EPD75_WaitDisplayReady();

	uint8_t VCEND	= wavedata[0] & 0x08;
	uint8_t BDEND	= (wavedata[1] & 0xC0) >> 6;
	uint8_t EVS		= VCEND | BDEND;
	uint8_t PLL		= (wavedata[0] & 0xF0) >> 4;
	uint8_t XON		= wavedata[2] & 0xC0;

    EPD_SendCommand(END_VOLTAGE_SETTINGS);	      			// EVS
    EPD_SendData(EVS);

    EPD_SendCommand(PLL_CONTROL);			  				// PLL setting
    EPD_SendData(PLL);

    EPD_SendCommand(POWER_SETTING);       					// Set VGH VGL VSH VSL VSHR
    EPD_SendData (0x17);
    EPD_SendData (wavedata[0] & 0x07);  					// VGH/VGL Voltage Level selection
    EPD_SendData (wavedata[1] & 0x3F);						// VSH for black
    EPD_SendData (wavedata[2] & 0x3F);  					// VSL for white
    EPD_SendData (wavedata[3] & 0x3F);  					// VSHR for red

    EPD_SendCommand(LUTOPT);	      						// LUTOPT
    EPD_SendData(XON);
    EPD_SendData(wavedata[4]);

    EPD_SendCommand(VCM_DC_SETTING_REGISTER);       		// VCOM_DC setting
    EPD_SendData(wavedata[5]);  							// Vcom value
}

static void EPD75_WaitDisplayReady(void) {
	EPD_SendCommand(GET_STATUS);
	while (true) {
    	if (HAL_GPIO_ReadPin(EPD_BUSY_GPIO_Port, EPD_BUSY_Pin) != GPIO_PIN_RESET)
    		break;
    }
}
