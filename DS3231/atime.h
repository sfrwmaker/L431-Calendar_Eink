/*
 * Time.h
 *
 *  Created on: June 13 2020
 *      Author: Alex
 *
 * low level time and date functions
 */

#ifndef _TIME_H_
#define _TIME_H_

#include "main.h"

#ifndef __cplusplus
#include <stdbool.h>
#endif

typedef uint32_t atime_t;

typedef enum {
    dowInvalid, dowSunday, dowMonday, dowTuesday, dowWednesday, dowThursday, dowFriday, dowSaturday
} timeDayOfWeek_t;

typedef enum {timeNotSet, timeNeedsSync, timeSet
}  timeStatus_t;

typedef enum {
    tmSecond, tmMinute, tmHour, tmWday, tmDay,tmMonth, tmYear, tmNbrFields
} tmByteFields;

typedef struct  {
	uint8_t		Second;
	uint8_t 	Minute;
	uint8_t 	Hour;
	uint8_t 	Wday;						// day of week, sunday is day 1
	uint8_t 	Day;
	uint8_t 	Month;						// January is 1
	uint8_t 	Year;   					// offset from 1970;
}  tmElements_t, TimeElements, *tmElementsPtr_t;

typedef bool (*getExternalTime)(tmElements_t *tm);

//convenience macros to convert to and from tm years
#define  tmYearToCalendar(Y) ((Y) + 1970)	// full four digit year
#define  CalendarYrToTm(Y)   ((Y) - 1970)
#define  tmYearToY2k(Y)      ((Y) - 30)		// offset is from 2000
#define  y2kYearToTm(Y)      ((Y) + 30)

/*==============================================================================*/
/* Useful Constants */
#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24UL)
#define DAYS_PER_WEEK (7UL)
#define SECS_PER_WEEK (SECS_PER_DAY * DAYS_PER_WEEK)
#define SECS_PER_YEAR (SECS_PER_WEEK * 52UL)
#define SECS_YR_2000  (946684800UL) // the time at the start of y2k

/* Useful Macros for getting elapsed time */
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN)
#define numberOfHours(_time_) (( _time_% SECS_PER_DAY) / SECS_PER_HOUR)
#define dayOfWeek(_time_)  ((( _time_ / SECS_PER_DAY + 4)  % DAYS_PER_WEEK)+1) // 1 = Sunday
#define elapsedDays(_time_) ( _time_ / SECS_PER_DAY)  // this is number of days since Jan 1 1970
#define elapsedSecsToday(_time_)  (_time_ % SECS_PER_DAY)   // the number of seconds since last midnight

// The following macros are used in calculating alarms and assume the clock is set to a date later than Jan 1 1971
// Always set the correct time before settting alarms
#define previousMidnight(_time_) (( _time_ / SECS_PER_DAY) * SECS_PER_DAY)  // time at the start of the given day
#define nextMidnight(_time_) ( previousMidnight(_time_)  + SECS_PER_DAY )   // time at the end of the given day
#define elapsedSecsThisWeek(_time_)  (elapsedSecsToday(_time_) +  ((dayOfWeek(_time_)-1) * SECS_PER_DAY) )   // note that week starts on day 1
#define previousSunday(_time_)  (_time_ - elapsedSecsThisWeek(_time_))      // time at the start of the week for the given time
#define nextSunday(_time_) ( previousSunday(_time_)+SECS_PER_WEEK)          // time at the end of the week for the given time


#ifdef __cplusplus
extern "C" {
#endif

uint8_t		hour(atime_t t);
uint8_t     minute(atime_t t);				// the minute now
uint8_t     second(atime_t t);				// the second for the given time
uint8_t     day(atime_t t);					// the day for the given time
uint8_t     weekday(atime_t t);				// the weekday for the given time
uint8_t     month(atime_t t);				// the month now  (Jan is month 1)
uint16_t	year(atime_t t);				// the year for the given time

atime_t		now();							// return the current time as seconds since Jan 1 1970
void    	setTime(atime_t t);
void		adjustTime(int32_t adjustment);

void		breakTime(atime_t time, tmElements_t* tm);  // break atime_t into elements
atime_t		makeTime(tmElements_t* tm);		// convert time elements into atime_t

timeStatus_t	timeStatus();
void			setSyncProvider(getExternalTime getTimeFunction);
void			setSyncInterval(atime_t interval);

#ifdef __cplusplus
}
#endif

#endif
