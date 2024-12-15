/*
 * DS3231.c
 *
 *  Created on: June 13 2020
 *      Author: Alex
 *  2024 AUG 25
 *  	Implemented ALARM support
 */

#include "DS3231.h"

// Forward static function declarations

static uint8_t decToNum(uint8_t dec) {
	return ((dec >> 4) * 10 + (dec & 0x0F));
}

static uint8_t numToDec(uint8_t num) {
	 return ((num / 10) << 4) | (num % 10);
}

bool DS3231_Read(tmElements_t *tm) {
	uint8_t rtc_buff[7];
	if (HAL_OK != HAL_I2C_Mem_Read(&DS3231_I2C, (RTC_ADDR << 1), 0x00, I2C_MEMADD_SIZE_8BIT, rtc_buff, sizeof(rtc_buff), 1000)) {
		return false;
	}
	uint8_t indx = 0;
	tm->Second	= decToNum(rtc_buff[indx++] & 0x7F);
	tm->Minute	= decToNum(rtc_buff[indx++] & 0x7F);
	if (rtc_buff[indx] & 0x40)	{								// AM/PM mode
		tm->Hour = decToNum(rtc_buff[indx] & 0x1F);
		if (rtc_buff[indx] & 0x20) tm->Hour += 12;
	} else {													// 24 Hour mode
		tm->Hour = decToNum(rtc_buff[indx] & 0x3F);
	}
	++indx;
	tm->Wday	= rtc_buff[indx++] & 0x07;
	tm->Day 	= decToNum(rtc_buff[indx++] & 0x3F);
	tm->Month	= decToNum(rtc_buff[indx++] & 0x1F);
	tm->Year	= decToNum(rtc_buff[indx]);
	return true;
}

bool DS3231_Write(tmElements_t *tm) {
	uint8_t rtc_buff[7];
	uint8_t indx = 0;
	rtc_buff[indx++]	= numToDec(tm->Second) & 0x7F;
	rtc_buff[indx++]	= numToDec(tm->Minute) & 0x7F;
	rtc_buff[indx++] 	= numToDec(tm->Hour)   & 0x3F;			// save time in 24 hour format
	rtc_buff[indx++]	= tm->Wday & 0x7;
	rtc_buff[indx++]	= numToDec(tm->Day) & 0x3F;
	rtc_buff[indx++]	= numToDec(tm->Month) & 0x3F;
	rtc_buff[indx]		= numToDec(tm->Year);

	return  (HAL_OK == HAL_I2C_Mem_Write(&DS3231_I2C, (RTC_ADDR << 1), 0x00, I2C_MEMADD_SIZE_8BIT, rtc_buff, sizeof(rtc_buff), 1000));
}

int16_t DS3231_Temperature(void) {
	uint8_t rtc_buff[2] = {0};

	if (HAL_OK != HAL_I2C_Mem_Read(&DS3231_I2C, (RTC_ADDR << 1), 0x11, I2C_MEMADD_SIZE_8BIT, rtc_buff, sizeof(rtc_buff), 1000)) {
		return -275;
	}

	int16_t value = (rtc_buff[0] << 8) | (rtc_buff[1]);
	value = (value >> 6);
	return (value >> 2);
}

bool DS3231_Set_Alarm(uint8_t alarm, DS3231_Alarm_Mask_t alarm_mask, tmElements_t *tm) {
	uint8_t rtc_buff[4];
	uint8_t indx = 0;
	alarm &= 1;
	rtc_buff[indx++]	= numToDec(tm->Second) & 0x7F;
	rtc_buff[indx++]	= numToDec(tm->Minute) & 0x7F;
	rtc_buff[indx++]	= numToDec(tm->Hour)   & 0x3F;			// save time in 24 hour format
	if (alarm_mask & 0x10) {									// match Week day, not date
		rtc_buff[indx]	= (numToDec(tm->Wday) & 7) | 0x40;		// The 6th bit
	} else {
		rtc_buff[indx]	= numToDec(tm->Day) & 0x3F;
	}
	if (alarm_mask & 0x08) rtc_buff[indx--]	|= 0x80;			// AxM4 bit: ignore date
	if (alarm_mask & 0x04) rtc_buff[indx--] |= 0x80;			// AxM3 bit: ignore hour
	if (alarm_mask & 0x02) rtc_buff[indx--]	|= 0x80;			// AxM2 bit: ignore minute
	if (alarm_mask & 0x01) rtc_buff[indx]	|= 0x80;			// AxM1 bit: ignore second
	if (alarm > 0) {											// ALARM2: no seconds
		if (HAL_OK != HAL_I2C_Mem_Write(&DS3231_I2C, (RTC_ADDR << 1), 0x0B, I2C_MEMADD_SIZE_8BIT, &rtc_buff[1], sizeof(rtc_buff)-1, 1000))
			return false;										// Failed to setup the ALARM2
	} else {													// ALARM1
		if (HAL_OK != HAL_I2C_Mem_Write(&DS3231_I2C, (RTC_ADDR << 1), 0x07, I2C_MEMADD_SIZE_8BIT, rtc_buff, sizeof(rtc_buff), 1000))
			return false;										// Failed to setup the ALARM1
	}
	// Read the control register
	if (HAL_OK == HAL_I2C_Mem_Read(&DS3231_I2C, (RTC_ADDR << 1), 0x0E, I2C_MEMADD_SIZE_8BIT, rtc_buff, 1, 1000)) {
		rtc_buff[0] |= (1 << alarm) | 0x02;						// Activate ALARM1 or ALARM2 and set INTR bit
		return (HAL_OK == HAL_I2C_Mem_Write(&DS3231_I2C, (RTC_ADDR << 1), 0x0E, I2C_MEMADD_SIZE_8BIT, rtc_buff, 1, 1000));
	}
	return false;
}

bool DS3231_SaveValue(uint8_t value) {
	if (value > 59)
		return false;
	value = numToDec(value) & 0x7F;
	return (HAL_OK == HAL_I2C_Mem_Write(&DS3231_I2C, (RTC_ADDR << 1), 0x0B, I2C_MEMADD_SIZE_8BIT, &value, 1, 1000));
}

uint8_t DS3231_RestoreValue(void) {
	uint8_t value = 0;
	if (HAL_OK == HAL_I2C_Mem_Read(&DS3231_I2C, (RTC_ADDR << 1), 0x0B, I2C_MEMADD_SIZE_8BIT, &value, 1, 1000)) {
		value = decToNum(value & 0x7F);
	}
	return value;
}

uint8_t DS3231_Alarm_Raised(void) {
	uint8_t status	= 0;
	uint8_t alarm	= 0;
	if (HAL_OK == HAL_I2C_Mem_Read(&DS3231_I2C, (RTC_ADDR << 1), 0x0F, I2C_MEMADD_SIZE_8BIT, &status, 1, 1000)) {
		alarm  = status & 0x3;
		status = ~alarm;										// Clear alarm flag
		HAL_I2C_Mem_Write(&DS3231_I2C, (RTC_ADDR << 1), 0x0F, I2C_MEMADD_SIZE_8BIT, &status, 1, 1000);
	}
	return alarm;
}
