/*
 * common.h
 *
 * 2024 FEB 22
 */

#ifndef E_INK_COMMON_H_
#define E_INK_COMMON_H_

#include "main.h"

#ifndef __cplusplus
#include <stdbool.h>
#endif

#define EPD_SPI_PORT		hspi1
extern SPI_HandleTypeDef 	EPD_SPI_PORT;

// Comment out next line to built more faster library functions EPD_DrawHLine() and EPD_DrawVLine()
// These function require more space
//#define TINY_LIB (1)

#define EPD_LAYERS	(2)

typedef enum { ROTATE_0	= 0, ROTATE_90 = 1, ROTATE_180 = 2, ROTATE_270 = 3 } Rotation_t;
typedef double (*LineThickness)(uint16_t pos, uint16_t length);
typedef enum disp_mode { MODE_CLEAR = 0, MODE_BLACK = 1, MODE_RED = 2, MODE_FULL = 3 } EPD_DISPLAY_MODE;
typedef uint8_t COLOR;

typedef struct sRectangle {
	uint16_t	x_upl;										// Upper left  corner X coordinate of rectangle
	uint16_t	y_upl;										// Upper left  corner Y coordinate of rectangle
	uint16_t	x_lor;										// Lower right corner X coordinate
	uint16_t	y_lor;										// Lower right corner Y coordinate
} Rectangle;


typedef struct {
	uint8_t	vcom_size;
	uint8_t ww_size;
	uint8_t bw_size;
	uint8_t wb_size;
	uint8_t bb_size;
	uint8_t lut_data[];
} LUT;

// epd display commands
typedef enum e_epd_cmd {
	PANEL_SETTING						= 0x00,
	POWER_SETTING						= 0x01,
	POWER_OFF							= 0x02,
	POWER_OFF_SEQUENCE_SETTING			= 0x03,
	POWER_ON							= 0x04,
	POWER_ON_MEASURE					= 0x05,
	BOOSTER_SOFT_START					= 0x06,
	DEEP_SLEEP							= 0x07,
	DATA_START_TRANSMISSION_1			= 0x10,
	DATA_STOP							= 0x11,
	DISPLAY_REFRESH						= 0x12,
	DATA_START_TRANSMISSION_2			= 0x13,
	PARTIAL_DATA_START_TRANSMISSION_1	= 0x14,
	PARTIAL_DATA_START_TRANSMISSION_2	= 0x15,
	PARTIAL_DISPLAY_REFRESH				= 0x16,
	LUT_FOR_VCOM 						= 0x20,
	LUT_WHITE_TO_WHITE					= 0x21,
	LUT_BLACK_TO_WHITE					= 0x22,
	LUT_WHITE_TO_BLACK					= 0x23,
	LUT_BLACK_TO_BLACK					= 0x24,
	LUTOPT								= 0x2a,
	PLL_CONTROL							= 0x30,
	TEMPERATURE_SENSOR_COMMAND			= 0x40,
	TEMPERATURE_SENSOR_CALIBRATION		= 0x41,
	TEMPERATURE_SENSOR_WRITE			= 0x42,
	TEMPERATURE_SENSOR_READ				= 0x43,
	VCOM_AND_DATA_INTERVAL_SETTING		= 0x50,
	LOW_POWER_DETECTION					= 0x51,
	END_VOLTAGE_SETTINGS				= 0x52,
	TCON_SETTING						= 0x60,
	TCON_RESOLUTION						= 0x61,
	SOURCE_AND_GATE_START_SETTING		= 0x62,
	GATE_SOURCE_GSST					= 0x65,
	GET_STATUS							= 0x71,
	AUTO_MEASURE_VCOM					= 0x80,
	VCOM_VALUE							= 0x81,
	VCM_DC_SETTING_REGISTER				= 0x82,
	PAARTIAL_PTL						= 0x90,
	PARTIAL_PTIN						= 0x91,
	PARTIAL_PTOUT						= 0x92,
	PROGRAM_MODE						= 0xA0,
	ACTIVE_PROGRAM						= 0xA1,
	READ_OTP_DATA						= 0xA2,
	POWER_OPTIMIZATION					= 0xF8
} EPD_CMD;

#ifdef __cplusplus
extern "C" {
#endif

// Initialize the memory structures for display
// Arguments:
//	w, h	- screen width and height (in pictures)
//  layers	- number of color layers in the display, 1 - for Black & White display, 2 = for Red, Black & White display
//  full	- flag indication to allocate separate memory buffer for each color layer, otherwise single buffer will be allocated
//			  and each layer should be displayed consequently (page by page)
bool		EPD_Init(uint16_t width, uint16_t height, uint8_t layers, bool full);
uint16_t	EPD_Width(void);
uint16_t	EPD_Height(void);
void		EPD_Clear(COLOR c);
void 		EPD_DrawPixel(uint16_t x, uint16_t y, COLOR c);
void 		EPD_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, COLOR c);
void 		EPD_DrawHLine(uint16_t x, uint16_t y, uint16_t line_width, COLOR c);
void 		EPD_DrawVLine(uint16_t x, uint16_t y, uint16_t line_height, COLOR c);
void 		EPD_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, COLOR c);
void 		EPD_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, COLOR c);
void		EPD_DrawRoundRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, COLOR c);
void		EPD_DrawFilledRoundRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, COLOR c);
void 		EPD_DrawCircle(uint16_t x, uint16_t y, uint8_t radius, COLOR c);
void 		EPD_DrawFilledCircle(uint16_t x, uint16_t y, uint8_t radius, COLOR c);
void		EPD_DrawBitmap(uint16_t x, uint16_t y, const uint8_t *bm, uint16_t bm_width, uint16_t bm_height, COLOR fg, COLOR bg);
void		EPD_SetRotation(Rotation_t r);
uint8_t		*EPD_GetLayerData(COLOR c);
uint8_t 	EPD_Layer(COLOR c);
uint8_t		EPD_GetLayerNumber(void);
uint8_t		EPD_GetActiveLayer(void);
bool		EPD_IsFullMemory(void);
bool		EPD_FirstPage(void);
bool		EPD_TurnPage(void);
bool		EPD_AbsRectangle(Rectangle *r);					// Translate rectangle coordinates to absolute display coordinate. Used to update display partially.

void 		EPD_DrawThickLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t thickness, COLOR color);
void		EPD_DrawVarThickLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, LineThickness thickness, COLOR color);

void		EPD_Reset(void);
void 		EPD_Sleep(void);
void		EPD_WaitDisplayReady(GPIO_PinState busy_status);
void 		EPD_SendCommand(EPD_CMD command);
void		EPD_SendData(uint8_t data);
void		EPD_SendDataBuff(const uint8_t *data, uint32_t len);
void		EPD_SetPartialRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void		EPD_SetPartialArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void		EPD_SetLut(const LUT *pLut);

#ifdef __cplusplus
}
#endif

#endif /* E_INK_COMMON_H_ */
