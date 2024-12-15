/*
 * moon.cpp
 *
 *  Created on: Oct 29, 2023
 *      Author: Alex
 */

#include "moon.h"

atime_t	MOON::sunRise(void) {
	if (now() >= next_day)
		updateSunTime();
	return sunrise;
}

atime_t	MOON::sunSet(void) {
	if (now() >= next_day)
		updateSunTime();
	return sunset;
}


uint8_t MOON::moonDay(void) {
	atime_t date = now();
	float IP = normalize((julianDay(date) - 2451550.1) / 29.530588853);
	IP *= 29.530588853;
	uint8_t age = (uint8_t)IP;
	return age + 1;
}

bool MOON::isDay(void) {
	atime_t n = now();										// GMT
	return (n + tz > sunRise() && n + tz < sunset);			// sunRise() and sunSet() return local time
}

void MOON::updateSunTime(void) {
	sunrise	= sunTime(true);
	sunset	= sunTime(false);
	next_day = sunset + 86400 - sunset %86400;
	next_day -= tz;
}

float MOON::normalize(float v) {
	v -= floor(v);
	if (v < 0) v += 1;
	return v;
}

uint32_t MOON::julianDay(atime_t date) {
	long y = year(date);
	long m = month(date);
	if (m > 2) {
		m = m - 3;
	} else {
		m = m + 9;
		y--;
	}
	long c = y / 100L;          							// Compute century
	y -= 100L * c;
	return ((uint32_t)day(date) + (c * 146097L) / 4 + (y * 1461L) / 4 + (m * 153L + 2) / 5 + 1721119L);
}

// Calculate the sunrise and sunset time
atime_t MOON::sunTime(bool sun_rise) {
	atime_t date = now() + tz;
	tmElements_t tm;
	breakTime(date, &tm);
	tm.Hour   = 12;
	tm.Minute = 0;
	tm.Second = 0;
	date = makeTime(&tm);
	date -= tz;												// Our noon GMT

	// first calculate the day of the year
	int N1 = 275 * (month(date)) / 9;
	int N2 = (month(date)+9)/12;
	int N3 = 1 + (year(date) - 4 * (year(date) / 4) + 2) / 3;
	int N = N1 - (N2 * N3) + day(date) - 30;

	// convert the longitude to hour value and calculate an approximate time
	float lngHour = longitude / 15.0;
	float t = 0;
	if (sun_rise)
		t = N + ((6 - lngHour) / 24);
	else
		t = N + ((18 - lngHour) / 24);

	// Sun's mean anomaly
	float M = (0.9856 * t) - 3.289;
	M *= toRad;

	// the Sun's true longitude
	float L = M + (1.916*toRad * sin(M)) + (0.020*toRad * sin(2 * M)) + 282.634*toRad;
	if (L < 0)      L += twoPi;
	if (L > twoPi) L -= twoPi;

	// the Sun's right ascension
	float RA = toDeg*atan(0.91764 * tan(L));
	if (RA < 0)   RA += 360;
	if (RA > 360) RA -= 360;

	// right ascension value needs to be in the same quadrant as L
	int Lquadrant  = (floor( L/M_PI_2)) * 90;
	int RAquadrant = (floor(RA/90)) * 90;
	RA += Lquadrant - RAquadrant;
	RA /= 15;         // right ascension value needs to be converted into hours

	// calculate the Sun's declination
	float sinDec = 0.39782 * sin(L);
	float cosDec = cos(asin(sinDec));

	// calculate the Sun's local hour angle
 	float cosH = (cos(zenith) - (sinDec * sin(latitude*toRad))) / (cosDec * cos(latitude*toRad));
 	if (cosH >  1) return 0;            // the Sun never rises on this location on the specified date
 	if (cosH < -1) return 0;            // the Sun never sets on this location on the specified date

 	// finish calculating H and convert into hours
 	float H = 0;
 	if (sun_rise)
 		H = 360 - toDeg*acos(cosH);
 	else
 		H = toDeg*acos(cosH);
 	H /= 15;

 	// calculate local mean time of rising/setting
 	float T = H + RA - (0.06571 * t) - 6.622;
 	if (T < 0)  T += 24;
 	if (T > 24) T -= 24;
 	float UT = T - lngHour;
 	float localT = UT + (float)tz / 3600.0;

 	tm.Hour = (uint8_t)localT;
 	localT -= tm.Hour;
 	localT *= 60;
 	tm.Minute = (uint8_t)localT;
 	localT -= tm.Minute;
 	localT *= 60;
 	tm.Second = (uint8_t)localT;

 	atime_t ret = makeTime(&tm);
 	return ret;
}
