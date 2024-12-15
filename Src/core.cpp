/*
 * core.cpp
 *
 */

#include <stdio.h>
#include "core.h"
#include "calendar.h"
#include "moon.h"
#include "DS3231.h"
#include "tools.h"
#include "button.h"

const uint8_t	display_layers = 2;								// The type of display; 1 - for Black & White, 2 - for Red, Black & White
const uint8_t	full_update_iteration = 6;						// Full screen update iteration
// Li-ion battery voltage range is [3.0-4.2]. TPS76333 voltage drop is 150 mv. So the battery range is [3.3- 4.2]
// To check the voltage the 1M/1M resistor divider used, the voltage range on ADC pin is [1.65 - 2.1]
const uint16_t batt_min = 2048;									// Minimum battery ADC reading
const uint16_t batt_max = 2500;									// Maximum battery ADC reading
volatile static uint32_t batt_adc	= 5000;						// Battery voltage readings

static CALENDAR dspl;
static DS3231	rtc;
static MOON		moon;											// Latitude, longitude and timezone set up in moon.h
static BTN		bt_menu(BTN_MENU_GPIO_Port, BTN_MENU_Pin);		// The menu button
static BTN		bt_plus(BTN_PLUS_GPIO_Port, BTN_PLUS_Pin);		// The plus button
static tmElements_t		tm = {0};
static tLANG	lang		= ENG_LANG;

extern ADC_HandleTypeDef	hadc1;

// Function to go Standby mode, saved for reference
static void goStandby(void) {
	for (uint8_t i = 0; i < 6; ++i) {
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		HAL_Delay(200);
	}
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);	// Turn-off the active LED
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);					// External interrupt on PA0 pin
	HAL_PWR_EnterSTANDBYMode();									// Never goes further
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	HAL_ResumeTick();
}

static void setAlarmNextMidnight(void) {
	tm.Second	= 0;											// Midnight
	tm.Minute	= 0;
	tm.Hour		= 0;
	rtc.setAlarm(0, DS3131_EVERY_DAY, &tm);
}

static bool waitADC(uint32_t ms) {
	for (uint32_t i = 0; i < ms; ++i) {
		if (batt_adc < 4096)
			return true;
		HAL_Delay(1);
	}
	return false;
}

static uint8_t readBatteryPcnt(uint32_t ms) {
	if (waitADC(ms)) {
		batt_adc = constrain(batt_adc, batt_min, batt_max);
		return map(batt_adc, batt_min, batt_max, 0, 100);
	}
	return 0;
}

static void refreshCalendar(void) {
    batt_adc	= 5000;
    HAL_ADC_Start_IT(&hadc1);									// Start measuring the battery voltage
	atime_t		sunrise = moon.sunRise();
	atime_t		sunset	= moon.sunSet();
	uint8_t		moonday	= moon.moonDay();
	if (dspl.firstPage()) {										// Prepare to draw the display page-by page
		do {
		    dspl.drawMonth(tm.Day, tm.Month, tmYearToCalendar(tm.Year));
			dspl.drawDate(tm.Day, tm.Month, tm.Wday-1);
		    dspl.drawSunTime(sunrise, sunset);
		    dspl.drawMoonPhase(moonday);
		    uint8_t b_pcnt = readBatteryPcnt(200);				// Wait to read the battery voltage
		    dspl.drawBattery(b_pcnt);
		} while (dspl.nextPage());
	} else {
		dspl.drawMonth(tm.Day, tm.Month, tmYearToCalendar(tm.Year));
		dspl.drawDate(tm.Day, tm.Month, tm.Wday-1);
		dspl.drawSunTime(sunrise, sunset);
		dspl.drawMoonPhase(moonday);
		uint8_t b_pcnt = readBatteryPcnt(200);					// Wait to read the battery voltage
		dspl.drawBattery(b_pcnt);
		dspl.display();
	}
}

static void finishClockSetup(void) {
	dspl.init(display_layers, false);							// Initialize display in default mode
	dspl.clear(MODE_FULL);
	refreshCalendar();
	dspl.hwPower(false);										// Turn-off the display
	setAlarmNextMidnight();										// Wake up next midnight
	goStandby();
}

static void changeTime(tmElements_t &tm, uint8_t item) {
	switch (item) {
		case 0:													// Hours
			if (++tm.Hour >= 24) tm.Hour = 0;
			break;
		case 1:													// Minute
			if (++tm.Minute >= 60) tm.Minute = 0;
			break;
		case 2:													// Day
			tm.Day = dspl.nextDay(tm);
			break;
		case 3:													// Month
			if (++tm.Month > 12) tm.Month = 1;
			break;
		case 4:													// Year
			if (++tm.Year > 130) tm.Year = 54;
			break;
		case 5:													// Language
		{
			uint8_t l = (uint8_t)lang;
			if (++l >= (uint8_t)LAST_LANG)
				l = ENG_LANG;
			lang = (tLANG)l;
			dspl.setLang(lang);
			break;
		}
		default:
			break;
	}
}

extern "C" void setup(void) {
	rtc.alarmRaised();											// Clear DS3231 alarm flag
	bt_plus.setTick(800);
	HAL_GPIO_WritePin(GPIOB, LED_Pin, GPIO_PIN_SET);			// Turn-on the active LED
	dspl.hwPower(true);
	uint8_t lang_code	= rtc.restoreValue();
	if (lang_code < (uint8_t)LAST_LANG)
		lang = (tLANG)lang_code;
	dspl.init(display_layers, false);							// Allocate single memory buffer for all display layers, display page-by-page
	dspl.setLang(lang);
	dspl.clear(MODE_FULL);


	if (!rtc.read(&tm)) {										// Failed to read the time from DS3231 RTC!
		dspl.msgHWerror();
		dspl.display();
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);	// Turn-off the active LED
		dspl.hwPower(false);									// Turn-off the display
		HAL_Delay(20000);
		goStandby();
	}
	if (tm.Year < 54) tm.Year = 54;
	atime_t	n	= makeTime(&tm);
	breakTime(n, &tm);											// Update the tm structure
	setTime(n);
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);	// Turn-off the active LED

	if (GPIO_PIN_SET == HAL_GPIO_ReadPin(REFRESH_GPIO_Port, REFRESH_Pin)) { // Wake-up menu button hold - wait for reprogram
		dspl.init(1, false);									// Initialize display in Black and White mode
		dspl.msgLoading();
		dspl.display();
		for (uint8_t i = 0; i < 120; ++i) {
			HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
			HAL_Delay(500);
		}
		finishClockSetup();										// Never goes further
	} else if (GPIO_PIN_SET == HAL_GPIO_ReadPin(BTN_MENU_GPIO_Port, BTN_MENU_Pin)) { // Menu button pressed - setup the clock
		dspl.clear(MODE_FULL);
		if (dspl.firstPage()) {									// Prepare to draw the display page-by page
			do {
				dspl.drawMonth(tm.Day, tm.Month, tmYearToCalendar(tm.Year));
			} while (dspl.nextPage());
		} else {												// Full-memory-buffer mode in case of Black and White display
			dspl.drawMonth(tm.Day, tm.Month, tmYearToCalendar(tm.Year));
			dspl.display();
		}
		dspl.activatePartialMode();
		dspl.initClockSetup();
		return;
	} else {
		refreshCalendar();
	}
	dspl.hwPower(false);										// Turn-off the display
	setAlarmNextMidnight();
	goStandby();
}

extern "C" void loop(void) {
	static uint32_t	b_pressed = HAL_GetTick();					// last time the button pressed
	static bool		refresh	= true;
	static uint8_t	item	= 0;

	uint8_t b_menu = bt_menu.buttonStatus();
	if (b_menu == 2) {											// Long press menu button - finish time setup
		atime_t	n	= makeTime(&tm);
		setTime(n);
		breakTime(n, &tm);										// Update the tm structure
		rtc.write(&tm);											// Update DS3231 clock
		rtc.saveValue((uint8_t)lang);							// Save current language to the second alarm
		finishClockSetup();
	} else if (b_menu == 1) {									// Short press menu button - switch to next item
		if (++item >= 6) item = 0;
		refresh 	= true;
		b_pressed 	= HAL_GetTick();
	} else {
		if (bt_plus.buttonStatus()) {							// Increment active item
			changeTime(tm, item);
			refresh = true;
			b_pressed 	= HAL_GetTick();
		}
	}

	if (refresh) {
		dspl.drawClockSetup(tm, item);
		refresh = false;
	}
	HAL_Delay(20);
	if (HAL_GetTick() - b_pressed > 30000) {
		rtc.read(&tm);
		finishClockSetup();
	}
}

/*
 * IRQ handler of ADC complete request. The data is in the ADC buffer (adc_buff)
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	if (hadc->Instance != ADC1) return;
	batt_adc = HAL_ADC_GetValue(hadc);
	HAL_ADC_Stop_IT(&hadc1);
}
