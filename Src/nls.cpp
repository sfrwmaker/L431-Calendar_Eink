/*
 * nls.cpp
 *
 *  Created on: 2024 AUG 31
 *      Author: Alex
 */

#include "nls.h"

const char*	ENG_MSG::weekDayShort(uint8_t wday) {
	if (wday > 6) wday = 6;
	return week_days_s[wday];
}

const char* ENG_MSG::weekDay(uint8_t wday) {
	if (wday > 6) wday = 6;
	return week_days[wday];
}

const char* ENG_MSG::month(uint8_t mon) {
	if (mon < 1)  mon = 1;
	if (mon > 12) mon = 12;
	return months[mon-1];
}


const char*	RUS_MSG::weekDayShort(uint8_t wday) {
	if (wday > 6) wday = 6;
	return week_days_s[wday];
}

const char* RUS_MSG::weekDay(uint8_t wday) {
	if (wday > 6) wday = 6;
	return week_days[wday];
}

const char* RUS_MSG::month(uint8_t mon) {
	if (mon < 1)  mon = 1;
	if (mon > 12) mon = 12;
	return months[mon-1];
}

std::string RUS_MSG::monthToday(uint8_t mon) {
	std::string month_td = std::string(month(mon));
	uint8_t len	= month_td.length();
	uint16_t last = month_td[len-2] << 8 | month_td[len-1];
	if (last == 0xd18c || last == 0xd0b9) {						// 'ь' , 'й'
		month_td[len-2] = 0xd1;									// 'я;
		month_td[len-1] = 0x8f;
	} else {
		month_td.append("а");
	}
	return month_td;
}
