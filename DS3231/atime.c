/*
 * Time.c
 *
 *  Created on: June 13 2020
 * low level time and date functions
 * Copyright (c) Michael Margolis 2009-2014
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <atime.h>

static tmElements_t tm;          			// a cache of time elements
static atime_t cache_time;   				// the time the cache was updated
static uint32_t sync_interval	= 300;		// time sync will be attempted after this many seconds

static uint32_t	sys_time		= 0;
static uint32_t prev_millis		= 0;
static uint32_t next_sync_time 	= 0;
static timeStatus_t Status		= timeNotSet;

#define LEAP_YEAR(Y)     ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )
static  const uint8_t monthDays[] = {31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0
getExternalTime getTimePtr;					// pointer to external sync function

// Forward static function declarations
static void refreshCache(atime_t t);

uint8_t	hour(atime_t t) {
	if (t == 0) t = now();
	refreshCache(t);
	return tm.Hour;
}

uint8_t minute(atime_t t) {
	if (t == 0) t = now();
	refreshCache(t);
	return tm.Minute;
}

uint8_t second(atime_t t) {
	if (t == 0) t = now();
	refreshCache(t);
	return tm.Second;
}

uint8_t day(atime_t t) {
	if (t == 0) t = now();
	refreshCache(t);
	return tm.Day;
}

uint8_t weekday(atime_t t) {
	if (t == 0) t = now();
	refreshCache(t);
	return tm.Wday;
}

uint8_t month(atime_t t) {
	if (t == 0) t = now();
	refreshCache(t);
	return tm.Month;
}

uint16_t year(atime_t t) {
	if (t == 0) t = now();
	refreshCache(t);
	return tmYearToCalendar(tm.Year);
}

atime_t now() {
	while (HAL_GetTick() - prev_millis >= 1000) {
		sys_time++;
		prev_millis += 1000;
	}
	if (next_sync_time <= sys_time) {
		if (getTimePtr != 0) {
			if (getTimePtr(&tm)) {
				atime_t t = makeTime(&tm);
				setTime(t);
			} else {
				next_sync_time = sys_time + sync_interval;
				Status = (Status == timeNotSet) ?  timeNotSet : timeNeedsSync;
			}
		}
	}
	return (atime_t)sys_time;
}

void setTime(atime_t t) {
	sys_time = (uint32_t)t;
	next_sync_time = (uint32_t)t + sync_interval;
	Status = timeSet;
	prev_millis = HAL_GetTick();
}

void adjustTime(int32_t adjustment) {
	sys_time += adjustment;
}

/*
 * break the given atime_t into time components
 * this is a more compact version of the C library localtime function
 * note that year is offset from 1970 !!!
 */
void breakTime(uint32_t timeInput, tmElements_t* tm) {
	uint32_t time = (uint32_t)timeInput;
	tm->Second = time % 60;
	time /= 60;									// now it is minutes
	tm->Minute = time % 60;
	time /= 60;									// now it is hours
	tm->Hour = time % 24;
	time /= 24;									// now it is days
	tm->Wday = ((time + 4) % 7) + 1;  			// Sunday is day 1

	uint8_t year = 0;
	uint32_t days = 0;
	while((uint32_t)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
		year++;
	}
	tm->Year = year;							// year is offset from 1970

	days -= LEAP_YEAR(year) ? 366 : 365;
	time -= days;								// now it is days in this year, starting at 0

	days = 0;
	uint8_t month = 0;
	uint8_t month_length = 0;
	for (month = 0; month < 12; ++month) {
		if (month == 1) {						// February
			if (LEAP_YEAR(year)) {
				month_length = 29;
			} else {
				month_length = 28;
			}
		} else {
			month_length = monthDays[month];
		}

		if (time >= month_length) {
			time -= month_length;
		} else {
			break;
		}
	}
	tm->Month = month + 1;						// jan is month 1
	tm->Day = time + 1;							// day of month
}

/*
 * assemble time elements into atime_t
 * note year argument is offset from 1970 (see macros in time.h to convert to other formats)
 * previous version used full four digit year (or digits since 2000),i.e. 2009 was 2009 or 9
 */
atime_t makeTime(tmElements_t *tm){

  	  // seconds from 1970 till 1 jan 00:00:00 of the given year
  	  uint32_t seconds= tm->Year*(SECS_PER_DAY * 365);
  	  for (int i = 0; i < tm->Year; ++i) {
  		  if (LEAP_YEAR(i)) {
  			  seconds +=  SECS_PER_DAY;			// add extra days for leap years
  		  }
  	  }

  	  // add days for this year, months start from 1
  	  for (int i = 1; i < tm->Month; ++i) {
  		  if ((i == 2) && LEAP_YEAR(tm->Year)) {
  			  seconds += SECS_PER_DAY * 29;
  		  } else {
  			  seconds += SECS_PER_DAY * monthDays[i-1];  //monthDay array starts from 0
  		  }
  	  }
  	  seconds += (tm->Day-1) * SECS_PER_DAY;
  	  seconds += tm->Hour * SECS_PER_HOUR;
  	  seconds += tm->Minute * SECS_PER_MIN;
  	  seconds += tm->Second;
  	  return (atime_t)seconds;
}

timeStatus_t timeStatus() {
	now();										// required to actually update the status
	return Status;
}

void setSyncProvider(getExternalTime getTimeFunction) {
	getTimePtr = getTimeFunction;
	next_sync_time = sys_time;
	now();										// this will sync the clock
}

void setSyncInterval(atime_t interval) { // set the number of seconds between re-sync
	sync_interval = (uint32_t)interval;
	next_sync_time = sys_time + sync_interval;
}

static void refreshCache(atime_t t) {
	if (t != cache_time) {
		breakTime(t, &tm);
		cache_time = t;
	}
}

