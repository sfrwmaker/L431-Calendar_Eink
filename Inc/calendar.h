/*
 * calendar.h
 *
 *  Created on: 2024 FEB 19
 */

#ifndef _CALENDAR_H_
#define _CALENDAR_H_

#include <string>
#include "epd.h"
#include "font.h"
#include "atime.h"
#include "nls.h"

class CALENDAR : public EPD75 {
	public:
		CALENDAR(void)	: EPD75()							{ }
		void			setLang(tLANG language = ENG_LANG);
		void			drawMonth(uint8_t day, uint8_t month, uint16_t year);
		void			drawDate(uint8_t day, uint8_t month, uint8_t wday);
		void			drawSunTime(atime_t sunrise, atime_t sunset);
		void 			drawMoonPhase(uint8_t phase);
		void			drawBattery(uint8_t batt_pcnt);
		void			initClockSetup(void);
		void			drawClockSetup(tmElements_t &tm, uint8_t item);
		uint8_t			nextDay(tmElements_t &tm);
		void			msgHWerror(void);
		void			msgLoading(void);
	private:
		void			drawMonthTitle(uint8_t month, uint16_t year);
		void			drawWeekDays(void);
		uint8_t			extraDays(uint8_t month, uint8_t s_wday); // Number of 'extra days' in the first week of the month
		std::string		monthRus(const std::string month);	// Translate russian month name to another form
		void			hhmm(atime_t sun_time, char tm[]);
		void			message(const char *msg);
		MSG				*msg		= 0;
		uint16_t		clock[6][2]	= {0};
		const uint16_t	start_x		= 240;
		const uint16_t  title_y		= 50;
		const uint16_t	week_days_y	= 110;
		const uint16_t	first_row_y	= 165;					// Where the month calendar begins
		const uint16_t	date_x		= 10;
		const uint16_t	date_y		= 250;
		const uint16_t	sun_x		= 55;
		const uint16_t	sun_y		= 20;
		const uint16_t  moon_x		= 65;
		const uint16_t	moon_y		= 480-100;
		const uint8_t	moon_d		= 80;					// Moon diameter
		const uint16_t	batt_x		= 5;
		const uint16_t	batt_y		= 480-120;
		const uint8_t	batt_length	= 100;
		const uint8_t	batt_width	= 30;
		const uint16_t	clock_y		= 20;					// Clock setup area starts here
		const uint16_t	clock_size	= 200;					// Clock setup area width, maximum partial display width
		const uint8_t	mdays[12]	= {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
};

#endif
