/*
 * DS3231.h
 *
 *  Created on: June 13, 2020
 *      Author: Alex
 */

#ifndef _DS3231_H_
#define _DS3231_H_

#include "main.h"

#define DS3231_I2C			hi2c1
extern I2C_HandleTypeDef 	DS3231_I2C;

#define RTC_ADDR			(0x68)

#ifndef __cplusplus
#include <stdbool.h>
#endif

typedef enum {
    DS3231_EVERY_SECOND		= 0x0F,
    DS3231_EVERY_MINUTE 	= 0x0E,						// Once a minute
    DS3231_EVERY_HOUR 		= 0x0C,						// match minutes *and* seconds
    DS3131_EVERY_DAY 		= 0x08,						// match hours *and* minutes, seconds
    DS3231_EVERY_MONTH 		= 0x00,						// match date *and* hours, minutes, seconds
    DS3231_EVERY_WEEK 		= 0x10						// match week day *and* hours, minutes, seconds
} DS3231_Alarm_Mask_t;

#ifdef __cplusplus
extern "C" {
#endif

#include "atime.h"

bool		DS3231_Read(tmElements_t *tm);
bool		DS3231_Write(tmElements_t *tm);
int16_t		DS3231_Temperature(void);
// Activate Alarm; alarm can be 0 for ALARM1 and 1 for ALARM2; ALARM2 cannot set the seconds
bool		DS3231_Set_Alarm(uint8_t alarm, DS3231_Alarm_Mask_t alarm_mask, tmElements_t *tm);
uint8_t		DS3231_Alarm_Raised(void);					// Return The ALARM has been raised: 1 - for ALARM1, 2 - For ALARM 2, 2 - for both alarms
bool		DS3231_SaveValue(uint8_t value);			// Save value 0-59 to the second alarm minutes
uint8_t 	DS3231_RestoreValue(void);					// Restore value 0-59 from the second alarm minutes

#ifdef __cplusplus
}

class DS3231 {
	public:
		DS3231()										{ }
		bool		read(tmElements_t *tm)				{ return DS3231_Read(tm);							}
		bool		write(tmElements_t *tm)				{ return DS3231_Write(tm);							}
		int16_t		temperature(void)					{ return DS3231_Temperature();						}
		bool		setAlarm(uint8_t alarm, DS3231_Alarm_Mask_t alarm_mask, tmElements_t *tm)
														{ return DS3231_Set_Alarm(alarm, alarm_mask, tm);	}
		uint8_t		alarmRaised(void)					{ return DS3231_Alarm_Raised();						}
		bool		saveValue(uint8_t value)			{ return DS3231_SaveValue(value);					}
		uint8_t 	restoreValue(void)					{ return DS3231_RestoreValue();						}
};

#endif

#endif
