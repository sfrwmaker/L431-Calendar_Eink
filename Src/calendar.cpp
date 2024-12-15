/*
 * calendar.cpp
 *
 *  Created on: 2024 FEB 19
 *      Author: Alex
 */

#include <stdio.h>
#include <math.h>
#include "calendar.h"
#include "atime.h"
#include "tools.h"
#include "nls.h"

static ENG_MSG eng_data;
static RUS_MSG rus_data;

static const uint8_t bm_sun[465] = { /* 96x38 */
	0X00,0X00,0X00,0X00,0X00,0X01,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X01,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0XC0,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0XC0,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X01,0XC0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X01,0XC0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0XC0,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0XC0,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X04,0X00,0X01,0XC0,0X00,0X10,0X00,0X00,0X00,0X00,0X00,0X00,0X0E,
	0X00,0X01,0XC0,0X00,0X38,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0X00,0X01,0XC0,0X00,
	0X70,0X00,0X00,0X00,0X00,0X00,0X00,0X07,0X00,0X00,0X00,0X00,0XE0,0X00,0X00,0X00,
	0X00,0X00,0X00,0X03,0X80,0X00,0X00,0X01,0XE0,0X00,0X00,0X00,0X00,0X00,0X00,0X01,
	0XC0,0X00,0X00,0X01,0XC0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XC0,0X3F,0XFF,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X43,0XFF,0XFF,0XE0,0X00,0X00,0X00,0X00,
	0X00,0X70,0X00,0X00,0X3F,0XE0,0X03,0XFE,0X00,0X00,0X0F,0X00,0X00,0X7E,0X00,0X00,
	0XFF,0X00,0X00,0X7F,0X00,0X00,0X7F,0X00,0X00,0X1F,0XC0,0X03,0XF0,0X00,0X00,0X0F,
	0XC0,0X01,0XFC,0X00,0X00,0X03,0XF0,0X07,0XC0,0X00,0X00,0X01,0XF0,0X07,0XC0,0X00,
	0X00,0X00,0XFC,0X0F,0X00,0X00,0X00,0X00,0XF8,0X3F,0X00,0X00,0X00,0X00,0X1C,0X3E,
	0X00,0X00,0X00,0X00,0X3C,0X3C,0X00,0X00,0X00,0X00,0X08,0X38,0X00,0X00,0X00,0X00,
	0X1E,0X00,0X00,0X00,0X00,0X00,0X00,0X78,0X00,0X00,0X00,0X00,0X0F,0X00,0X00,0X00,
	0X00,0X00,0X00,0XF0,0X00,0X00,0X00,0X00,0X07,0X00,0X00,0X00,0X00,0X00,0X00,0XE0,
	0X00,0X00,0X00,0X00,0X07,0X80,0X00,0X00,0X00,0X00,0X00,0XE0,0X00,0X00,0X00,0X00,
	0X03,0X80,0X00,0X00,0X00,0X00,0X01,0XC0,0X00,0X00,0X00,0X00,0X01,0XC0,0X00,0X00,
	0X00,0X00,0X01,0XC0,0X00,0X00,0X00,0X00,0X01,0XC0,0X00,0X00,0X00,0X00,0X01,0XC0,
	0X00,0X00,0X00,0X00,0X01,0XC0,0X00,0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X03,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XC0,
	0X03,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XC0,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00
};

static const uint8_t bm_moon[]  = { /* 80x80 */
  0xff, 0xff, 0xff, 0xff, 0xe0, 0x07, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0x90, 0x7f, 0xfe, 0x0b, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xfe, 0x07, 0xff, 0xff, 0xf0, 0x7f, 0xff, 0xff,
  0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff,
  0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff,
  0xff, 0xff, 0xc1, 0xff, 0xff, 0xff, 0xff, 0x83, 0xff, 0xff,
  0xff, 0xff, 0x87, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xff, 0xff,
  0xff, 0xfe, 0x0b, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff,
  0xff, 0xfc, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xfc, 0x3f, 0xff,
  0xff, 0xf8, 0x07, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x1f, 0xff,
  0xff, 0xe0, 0x0f, 0xff, 0xbf, 0xff, 0xff, 0xff, 0x8f, 0xff,
  0xff, 0xe0, 0x0f, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xc7, 0xff,
  0xff, 0xc0, 0x7f, 0xfb, 0xff, 0xff, 0xff, 0xff, 0x67, 0xff,
  0xff, 0x80, 0x7c, 0x03, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xff,
  0xff, 0x00, 0x79, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xff,
  0xff, 0x00, 0xf8, 0x07, 0xff, 0xff, 0xff, 0xff, 0xf1, 0xff,
  0xfe, 0x00, 0xf0, 0x07, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff,
  0xfc, 0x00, 0x60, 0x07, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x7f,
  0xfc, 0x00, 0xc0, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f,
  0xf8, 0x00, 0x00, 0x0f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0x3f,
  0xf8, 0x00, 0x00, 0x13, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xbf,
  0xf0, 0x00, 0x10, 0x13, 0xbf, 0xe7, 0x77, 0xff, 0xff, 0xdf,
  0xe0, 0x00, 0x00, 0x07, 0xff, 0xf6, 0x77, 0xff, 0xff, 0xcf,
  0xe0, 0x00, 0x60, 0x36, 0xff, 0xff, 0xf1, 0xff, 0xff, 0xe7,
  0xc0, 0x10, 0xc6, 0x3f, 0xff, 0xfb, 0xf1, 0xff, 0xff, 0xe7,
  0xc0, 0x3c, 0x04, 0x3f, 0xff, 0xdf, 0xe9, 0xff, 0xf8, 0x63,
  0xe0, 0x70, 0x03, 0xbf, 0xff, 0xc7, 0xe9, 0xff, 0xf0, 0x27,
  0xc0, 0x10, 0x33, 0xfe, 0xff, 0x81, 0xc5, 0xff, 0xe0, 0x33,
  0xc0, 0x00, 0x7f, 0xff, 0xff, 0xc5, 0xc8, 0xff, 0xf0, 0x13,
  0x80, 0x00, 0x7f, 0xff, 0xff, 0xe9, 0xd9, 0xff, 0xf0, 0x11,
  0x80, 0x08, 0x7f, 0xff, 0xff, 0xfd, 0xc0, 0xf3, 0xf8, 0x31,
  0x80, 0x0c, 0x7f, 0xff, 0xff, 0xdf, 0xe0, 0x73, 0xf0, 0x39,
  0x80, 0x0e, 0xff, 0xff, 0xff, 0xff, 0xe8, 0x03, 0xf8, 0x71,
  0x00, 0x0e, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x01, 0xfc, 0xf1,
  0x00, 0x0f, 0x7f, 0xff, 0xdf, 0x3f, 0x80, 0x00, 0x7f, 0xe1,
  0x00, 0x0f, 0x7f, 0xff, 0x8c, 0x3f, 0x82, 0x00, 0xff, 0xe0,
  0x00, 0x1f, 0xff, 0xff, 0xbf, 0x1f, 0x80, 0x01, 0xff, 0xc4,
  0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x7f, 0xe8,
  0x00, 0x1f, 0xff, 0xfe, 0x7f, 0xff, 0x80, 0x00, 0xf3, 0xe0,
  0x00, 0x0f, 0xff, 0xdc, 0x7f, 0xff, 0xc0, 0x09, 0xe0, 0x48,
  0x00, 0x17, 0xff, 0xdc, 0xff, 0xff, 0xc0, 0x70, 0xc0, 0x00,
  0x00, 0x0d, 0xff, 0xfe, 0xff, 0xff, 0xe0, 0x39, 0x80, 0x38,
  0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x3f, 0x80, 0x38,
  0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0x00, 0x31,
  0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x80, 0x31,
  0x80, 0x00, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0xf9,
  0x90, 0x00, 0x7b, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xf9,
  0x90, 0x01, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe3, 0xf9,
  0x80, 0x00, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe3, 0xf1,
  0xd8, 0x00, 0xf3, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3,
  0xcc, 0x08, 0x73, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe3,
  0xe6, 0x2c, 0x21, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xe7,
  0xe7, 0x3e, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc7,
  0xc7, 0x3f, 0x1f, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc7,
  0xe7, 0xff, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf,
  0xe7, 0xfe, 0x3f, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf,
  0xf7, 0xf8, 0x7f, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf,
  0xfb, 0xf0, 0x7f, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf,
  0xfb, 0xf8, 0x3f, 0xf5, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf,
  0xfd, 0xf8, 0x1b, 0xd1, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f,
  0xfe, 0xf8, 0x3d, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xfe, 0x7e, 0x3f, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xff,
  0xff, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xff,
  0xff, 0x0f, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xff,
  0xff, 0x9f, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xff,
  0xff, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xff,
  0xff, 0xe1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xff,
  0xff, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x9f, 0xff,
  0xff, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xff,
  0xff, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff,
  0xff, 0xff, 0x17, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xff, 0xff,
  0xff, 0xff, 0xcb, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xff, 0xff,
  0xff, 0xff, 0xf1, 0xff, 0xff, 0xff, 0xff, 0xef, 0xff, 0xff,
  0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xff, 0xff,
  0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xfd, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xa7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xe7, 0xf7, 0xff, 0xff, 0xff, 0xff
};

void CALENDAR::setLang(tLANG language) {
	switch(language) {
		case RUS_LANG:
			msg = &rus_data;
			break;
		case ENG_LANG:
		default:
			msg	= &eng_data;
			break;
	}
}

void CALENDAR::drawMonth(uint8_t day, uint8_t month, uint16_t year) {
	tmElements_t tm;
	tm.Day 		= day;
	tm.Month 	= month;
	tm.Hour		= 0;
	tm.Minute	= 0;
	tm.Second	= 0;
	tm.Year		= CalendarYrToTm(year);
	atime_t	tmp	= makeTime(&tm);
	breakTime(tmp, &tm);

	drawMonthTitle(month, year);
	drawWeekDays();

	tm.Day 		= 1;
	tmp	= makeTime(&tm);
	breakTime(tmp, &tm);

	int8_t extra_days = extraDays(month, tm.Wday-1);
	uint8_t days = extra_days +  mdays[month-1];
	if (month == 2 && (year & 3) == 0)							// Leap year has 29 days in February
		++days;
	uint8_t rows = days / 7;
	if (days % 7) ++rows;										// The number of horizontal week rows in month 4 - 6
	if (rows < 5) rows = 5;										// Add extra row at the bottom
	uint16_t y_size = height() - first_row_y - 10;
	uint16_t y_step = y_size / (rows-1);
	uint16_t x_size = width() - start_x;
	uint16_t x_step = x_size / 7;

	setFont(u8g2_font_kam48ne);
	uint8_t h = getFontHeight();
	extra_days = ~(extra_days-1) + 1;							// extra_days = -extra_days + 1
	uint8_t row = 0;
	uint8_t col = 0;
	uint8_t start_wday = msg->startWeekDay();
	uint8_t sunday_pos = start_wday;							// The position of Sunday
	if (start_wday != 0) {
		sunday_pos = 7 - start_wday;
	}
	char num[4];
	for (int8_t d = extra_days; d <= mdays[month-1]; ++d) {
		if (d > 0) {
			sprintf(num, "%2d", d);
			uint16_t x = start_x + col * x_step;
			uint16_t y = first_row_y + row * y_step;
			COLOR clr = MODE_BLACK;
			if (EPD75::layers() > 1 && col == sunday_pos)
				clr = MODE_RED;
			if (d == day) {										// The current day
				uint8_t w = getStrWidth(num);
				drawFilledRoundRect(x - 3, y - h + 6, w + 6, h + 2, 5,  clr);
				drawStr(x, y, num, MODE_CLEAR);
			} else {
				uint8_t w = drawStr(x, y, num, clr);
				if (col == sunday_pos) {
					if (EPD75::layers() == 1) {					// Draw sundays in grayed color
						for (uint16_t xx = x; xx < x + w; x += 2) {
							drawVLine(x, y-h+5, h, MODE_CLEAR);
						}
					}
				}
			}
		}
		if (++col > 6) {										// End of week
			col = 0;
			++row;
		}
	}
}

void CALENDAR::msgHWerror(void) {
	if (!msg) msg = &eng_data;
	const char* m = msg->hhError();
	message(m);
}

void CALENDAR::msgLoading(void) {
	if (!msg) msg = &eng_data;
	const char* m = msg->loading();
	message(m);
}

void CALENDAR::drawBattery(uint8_t batt_pcnt) {
	drawDrawRoundRect(batt_x, batt_y, batt_width, batt_length+1, 3, MODE_BLACK); // Draw body
	uint16_t plus_x = (batt_width - 9) >> 1; plus_x += batt_x;
	drawFilledRoundRect(plus_x, batt_y-5, 9, 5, 2, MODE_BLACK);	// "+" terminal
	drawFilledRectangle(plus_x, batt_y-3, 9, 3, MODE_BLACK);
	uint16_t pcnt_len = map(batt_pcnt, 0, 100, 0, batt_length);
	if (pcnt_len > 0)
		drawFilledRoundRect(batt_x+2, batt_y+batt_length-pcnt_len, batt_width-6, pcnt_len, 2, MODE_BLACK);
	for (uint8_t i = 1; i < 5; ++i) {							// Draw separators
		uint16_t part = (batt_length * i) / 5;
		if (part < pcnt_len) {
			drawHLine(batt_x+1, batt_y+batt_length-part, batt_width-2, MODE_CLEAR);
		}
	}
}

void CALENDAR::drawSunTime(atime_t sunrise, atime_t sunset) {
	setFont(u8g2_font_ubuntu16u);
	uint16_t h		= getMaxCharHeight();
	char sr_time[8], ss_time[8];
	hhmm(sunrise, sr_time);
	hhmm(sunset,  ss_time);
	uint16_t w_sr = getStrWidth(sr_time);
	uint16_t w = w_sr + getStrWidth(sr_time) + 20;
	uint16_t middle = sun_x + 96/2;
	w >>= 1;						// Half-width
	uint16_t x = 0;
	if (middle > w) x = middle - w;
	drawUTF8(x, sun_y + h +38+5, sr_time, MODE_BLACK);
	drawUTF8(x+w_sr+20, sun_y + h +38+5, ss_time, MODE_BLACK);
	drawBitmap(sun_x, sun_y, bm_sun, 96, 38, MODE_BLACK, MODE_CLEAR);

}

void CALENDAR::drawMoonPhase(uint8_t phase) {
	if (phase > 30) phase = 30;
	drawBitmap(moon_x, moon_y, bm_moon, moon_d, moon_d, MODE_CLEAR, MODE_BLACK);
	float cosA = cos(M_PI * phase / 15.0);
	for (uint16_t a = 0; a <= moon_d/2; ++a) {
		float b = sqrt(moon_d*a - a*a);
		uint16_t x1, x2;										// Area between x1 and x2 is not visible
		if (phase <= 15) {										// First half of moon
			x1 = round((float)moon_d/2 - b);					// Left border is a left edge of moon
			x2 = round((float)moon_d/2 + b*cosA);				// Right border depends of moon phase
			drawHLine(moon_x, moon_y+a, x2, MODE_CLEAR);		// Clear from icon start to right border (upper half of moon)
			drawPixel(moon_x+x2, moon_y+a, MODE_BLACK);			// Draw Right edge (upper half)
			if (a < moon_d-a) {
				drawHLine(moon_x, moon_y+moon_d-a, x2, MODE_CLEAR); // Clear from icon start to right border (lower half of moon)
				drawPixel(moon_x+x2, moon_y+moon_d-a, MODE_BLACK); // Draw Right edge (lower half)
			}
		} else {												// Second half of moon
			x1 = round((float)moon_d/2 - b*cosA);				// Left border depends of moon phase
			x2 = round((float)moon_d/2 + b);					// Right border is a left edge of moon
			drawHLine(moon_x+x1, moon_y+a, moon_d-x1, MODE_CLEAR); // Clear from right border to icon end (upper half of moon)
			drawPixel(moon_x+x1, moon_y+a, MODE_BLACK);			// Draw Left edge (upper half)
			if (a < moon_d-a) {
				drawHLine(moon_x+x1, moon_y+moon_d-a, moon_d-x1, MODE_CLEAR); // Clear from right border to icon end (lower half of moon)
				drawPixel(moon_x+x1, moon_y+moon_d-a, MODE_BLACK); // Draw Left edge (lower half)
			}
		}
	}

}

void CALENDAR::drawMonthTitle(uint8_t month, uint16_t year) {
	char y[6];
	sprintf(y, "%4d", year);

	setFont(u8g2_font_kam48ne);
	uint16_t w = getStrWidth(y);
	setFont(u8g2_font_ubuntu48);
	const char* mon = msg->month(month);
	w += getUTF8Width(mon);
	w += 20;
	uint16_t offset = (width() - start_x - w) >> 1;
	offset += start_x;
	drawHLine(offset, title_y + 5, w, MODE_BLACK);
	drawHLine(offset, title_y + 6, w, MODE_BLACK);

	w = drawUTF8(offset, title_y, mon, MODE_BLACK);
	offset += w + 20;
	setFont(u8g2_font_kam48ne);
	drawStr(offset, title_y, y, MODE_BLACK);
}

void CALENDAR::drawWeekDays(void) {
	setFont(u8g2_font_ubuntu48);
	uint16_t area_size = width() - start_x;
	uint16_t step = area_size / 7;
	uint8_t start_wday =  msg->startWeekDay();
	for (uint8_t d = 0; d < 7; ++d) {
		uint8_t wday = (d + start_wday);
		if (wday > 6) wday = 0;
		COLOR clr = MODE_BLACK;
		if (EPD75::layers() > 1 && wday == 0)
			clr = MODE_RED;
		drawUTF8(start_x + step * d, week_days_y, msg->weekDayShort(wday), clr);
	}
}

void CALENDAR::drawDate(uint8_t day, uint8_t month, uint8_t wday) {
	COLOR clr = MODE_BLACK;
	if (EPD75::layers() > 1 && wday == 0)
		clr = MODE_RED;
	char num[4];
	sprintf(num, "%d", day);
	setFont(u8g2_font_kam63n);
	setFontScale(2);
	uint16_t area_width = start_x - 10;							// start_x coordinate of the month calendar
	uint16_t w = getStrWidth(num);
	uint16_t offset = (area_width - w) >> 1;
	if (w > area_width) offset = 0;
	drawStr(offset, date_y, num, clr);

	std::string mon = msg->monthToday(month);
	mon.append(",");
	setFont(u8g2_font_ubuntu16u);
	setFontScale(2);
	uint8_t  h = getFontHeight();
	w = getUTF8Width(mon.c_str());
	offset = (area_width - w) >> 1;
	if (w > area_width) offset = 0;
	uint16_t y = date_y + h*2;
	drawUTF8(offset, y, mon.c_str(), clr);

	setFontScale(1);
	h = getFontHeight();
	const char* week_day = msg->weekDay(wday);
	w = getUTF8Width(week_day);
	offset = (area_width - w) >> 1;
	if (w > area_width) offset = 0;
	drawUTF8(offset, y + h + 4, week_day, clr);
}

// month number 1 - 12
// s_wday - week day of 1-st month day: 0 - sunday, 6 - saturday
uint8_t	CALENDAR::extraDays(uint8_t month, uint8_t s_wday) {
	if (s_wday > 6) s_wday = 6;
	if (month > 12) month = 12;
	int8_t days = 7;
	uint8_t start_wday = msg->startWeekDay();
	if (start_wday <= s_wday) {
		days = s_wday - start_wday;
    } else {
		days += s_wday;
		days -= start_wday;
	}
	return days;
}

void CALENDAR::hhmm(atime_t sun_time, char tm[]) {
	sun_time += 30;												// Round to minutes
	sun_time /= 60;												// Minutes
	uint8_t minutes = sun_time % 60;
	sun_time /= 60;												// Hours
	uint8_t hours	= sun_time % 24;
	sprintf(tm, "%02d:%02d", hours, minutes);
}

void CALENDAR::message(const char* msg) {
	setFont(u8g2_font_ubuntu48);
	uint16_t w = getUTF8Width(msg);
	w += 20;
	uint16_t offset = (width() - w) >> 1;
	drawUTF8(offset, title_y, msg, MODE_BLACK);
}

// Initialize clock items coordinates
void CALENDAR::initClockSetup(void) {
	setFont(u8g2_font_ubuntu16u);
	uint16_t h = getMaxCharHeight() + 6;
	clock[0][1] = clock[1][1] = clock_y + h;					// Y-coordinates of Hour and Minutes
	uint16_t w = getStrWidth("00 : 00") + 10;					// Clock size
	if (w > clock_size) {
		clock[0][0]	= 10;										// Hour X coordinate
		clock[1][0] = 96;										// Minute X coordinate
	} else {
		clock[0][0] = (clock_size - w) >> 1;					// Hour X coordinate
		clock[1][0] = clock[0][0] + (w >> 1);					// Minute X coordinate
	}

	clock[2][1] = clock[3][1] = clock_y + h + h;				// Y-coordinates of Day and Month
	w = getStrWidth("88 ");
	const char* lmn = msg->longestMonthName();
	uint16_t w1 = getUTF8Width(lmn);
	if (w+w1 > clock_size) {
		clock[2][0]	= 10;										// Day X coordinate
		clock[3][0] = w;										// Month X coordinate
	} else {
		clock[2][0] = (clock_size - w-w1) >> 1;					// Day X coordinate
		clock[3][0] = clock[2][0] + w+10;						// Month X coordinate
	}
	clock[4][1] = clock_y + 3*h;								// Y-coordinates of Year
	w = getStrWidth("8888");
	clock[4][0] = (clock_size - w) >> 1;						// Year X coordinate
	clock[5][1] = clock_y + 4*h;								// Y-coordinate of the language
	clock[5][0] = 0;											// not used
}

void CALENDAR::drawClockSetup(tmElements_t &tm, uint8_t item) {
    clear(MODE_FULL);
	drawDrawRoundRect(8, clock_y, clock_size-16, clock[5][1] - clock_y + 5, 5, MODE_BLACK);
	setFont(u8g2_font_ubuntu16u);
	uint8_t h = getFontHeight();
	char buf[10];

    uint8_t a = 0;
	sprintf(buf, "%02d", tm.Hour);
	uint16_t h_w = getStrWidth(buf);
	if (item == a)												// The active item is Hour
		drawFilledRoundRect(clock[a][0], clock[a][1]-h+6, h_w + 6, h + 2, 3,  MODE_BLACK);
	COLOR clr = (item == a)?MODE_CLEAR:MODE_BLACK;
	drawStr(clock[a][0], clock[a][1], buf, clr);				// Hour
    drawStr(clock[a][0] + h_w+4, clock[0][1], ":", MODE_BLACK);	// Semicolon

    ++a;
    sprintf(buf, "%02d", tm.Minute);
	if (item == a)												// The active item is Minutes
		drawFilledRoundRect(clock[a][0], clock[a][1]-h+6, h_w + 6, h + 2, 3,  clr);
    clr = (item == a)?MODE_CLEAR:MODE_BLACK;
    drawStr(clock[a][0], clock[a][1], buf, clr);				// Minutes

    ++a;
    sprintf(buf, "%02d", tm.Day);
    uint16_t w = getStrWidth(buf);
	if (item == a)												// The active item is Day
		drawFilledRoundRect(clock[a][0], clock[a][1]-h+6, w + 6, h + 2, 3,  MODE_BLACK);
	clr = (item == a)?MODE_CLEAR:MODE_BLACK;
    drawStr(clock[a][0], clock[a][1], buf, clr);				// Day

    ++a;
    const char* mon = msg->month(tm.Month);
    w = getUTF8Width(mon);
    if (item == a)												// The active item is Month
    	drawFilledRoundRect(clock[a][0], clock[a][1]-h+6, w + 6, h + 2, 3,  MODE_BLACK);
    clr = (item == a)?MODE_CLEAR:MODE_BLACK;
    drawUTF8(clock[a][0], clock[a][1], mon, clr); 				// Month

    ++a;
    sprintf(buf, "%4d", tmYearToCalendar(tm.Year));
    uint16_t y_w = getStrWidth(buf);
    if (item == a)												// The active item is Month
        drawFilledRoundRect(clock[a][0], clock[a][1]-h+6, y_w + 6, h + 2, 3,  MODE_BLACK);
    clr = (item == a)?MODE_CLEAR:MODE_BLACK;
    drawStr(clock[a][0], clock[a][1], buf, clr);				// Year

    ++a;
    uint16_t l_w = getUTF8Width(msg->name());
    uint16_t l_x = 0;
    if (clock_size >= l_w)
    	l_x = (clock_size - l_w) >> 1;
    if (item == a)												// The active item is Month
        drawFilledRoundRect(l_x, clock[a][1]-h+6, l_w + 6, h + 2, 3,  MODE_BLACK);
    clr = (item == a)?MODE_CLEAR:MODE_BLACK;
    drawUTF8(l_x, clock[a][1], msg->name(), clr);				// language

    displayPartial(0, clock_y, clock_size, clock[5][1] - clock_y + 10);
}

uint8_t CALENDAR::nextDay(tmElements_t &tm) {
	uint8_t m_days = mdays[tm.Month-1];
	uint8_t nxt = tm.Day + 1;
	if (nxt > m_days) nxt = 1;
	return nxt;
}
