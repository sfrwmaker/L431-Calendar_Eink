/*
 * nls.h
 *
 *  Created on: 2024 AUG 31
 *      Author: Alex
 */

#ifndef _NLS_H_
#define _NLS_H_

#include <string>

typedef enum e_lang {
	ENG_LANG	= 0, RUS_LANG, LAST_LANG
} tLANG;

class MSG {
	public:
		MSG()												{ }
		virtual ~MSG()										{ }
		virtual const char*	weekDayShort(uint8_t wday)		= 0;
		virtual const char* weekDay(uint8_t wday)			= 0;
		virtual const char* month(uint8_t mon)				= 0;
		virtual std::string monthToday(uint8_t mon)			= 0;
		virtual const char* longestMonthName(void)			= 0;
		virtual const char* hhError(void)					= 0;
		virtual const char* loading(void)					= 0;
		virtual const char*	name(void)						= 0;
		virtual uint8_t		startWeekDay(void)				= 0;
};

class ENG_MSG : public MSG {
	public:
		ENG_MSG()											{ }
		virtual const char*	weekDayShort(uint8_t wday);
		virtual const char* weekDay(uint8_t wday);
		virtual const char* month(uint8_t mon);
		virtual std::string monthToday(uint8_t mon)			{ return std::string(month(mon));	}
		virtual const char* longestMonthName(void)			{ return months[8];					}
		virtual const char* hhError(void)					{ return hw_error;					}
		virtual const char* loading(void)					{ return load_msg;					}
		virtual const char*	name(void)						{ return "English";					}
		virtual uint8_t		startWeekDay(void)				{ return 0;							} // Sunday
	private:
		const char*	week_days_s[7]	= {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};
		const char*	week_days[7]	= {"sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday" };
		const char*	months[12] 		= {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
		const char* hw_error		= "Hardware error";
		const char* load_msg		= "Waiting to update";
};

class RUS_MSG : public MSG {
	public:
		RUS_MSG()											{ }
		virtual const char*	weekDayShort(uint8_t wday);
		virtual const char* weekDay(uint8_t wday);
		virtual const char* month(uint8_t mon);
		virtual std::string monthToday(uint8_t mon);
		virtual const char* longestMonthName(void)			{ return months[8];					}
		virtual const char* hhError(void)					{ return hw_error;					}
		virtual const char* loading(void)					{ return load_msg;					}
		virtual const char*	name(void)						{ return "Русский";					}
		virtual uint8_t		startWeekDay(void)				{ return 1;							}	// Monday
	private:
		const char*	week_days_s[7]	= {"Вс", "Пн", "Вт", "Ср", "Чт", "Пт", "Сб"};
		const char*	week_days[7]	= {"воскресенье", "понедельник", "вторник", "среда", "четверг", "пятница", "суббота" };
		const char*	months[12] 		= {"Январь", "Февраль", "Март", "Апрель", "Май", "Июнь", "Июль", "Август", "Сентябрь", "Октябрь", "Ноябрь", "Декабрь"};
		const char* hw_error		= "Аппаратная ошибка";
		const char* load_msg		= "Ожидание обновления";
};

#endif /* _NLS_H_ */
