/*
 * moon.h
 *
 *  Created on: Oct 29, 2023
 *      Author: Alex
 */

#ifndef _MOON_H_
#define _MOON_H_

#include <stdint.h>
#include <math.h>
#include "atime.h"

class MOON {
	public:
		MOON(void)											{ }
		virtual			~MOON(void)							{ }
		int32_t			getTimeZone(void)					{ return tz;						}
		void			setTimeZone(int32_t tz)				{ this->tz = tz;					}
		float			getLatitude(void)					{ return latitude;					}
		float			getLongitude(void)					{ return longitude;					}
		int16_t			getAltitude(void)					{ return altitude; 					}
		uint8_t 		moonDay(void);						// 1..30
		atime_t			sunRise(void);
		atime_t			sunSet(void);
		float 			normalize(float v);
		uint32_t 		julianDay(atime_t date);
		bool			isDay(void);
	protected:
		atime_t			sunrise	= 0;						// Sun rise and sun set times in Local time!
		atime_t			sunset	= 0;
		atime_t			next_day = 0;						// The time of the next day, tomorrow
		float			latitude	= 55.6081560235811;		// The coordinates from current weather forecast
		float			longitude 	= 37.7425667081002;
		int16_t			altitude	= 160;					// Sea level altitude (meters); Initialize with wrong value
		int32_t			tz			= 10800;				// Timezone from current weather conditions, seconds
	private:
		void			updateSunTime(void);
		atime_t 		sunTime(bool sun_rise);
		const float toRad = M_PI/180.0;
		const float toDeg = 180.0/M_PI;
		const float twoPi = 2 * M_PI;
		const float zenith = 90.83 * toRad;
};

#endif
