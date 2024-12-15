#ifndef __ENCODER_H
#define __ENCODER_H
#include "main.h"
#include <stdint.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

typedef struct _class_RENC RENC;

#ifdef __cplusplus
extern "C" {
#endif

void 	RENC_createButton(RENC* renc, GPIO_TypeDef* ButtonPORT, uint16_t ButtonPIN);
void 	RENC_setTimeout(RENC* renc, uint16_t timeout_ms);
bool    RENC_setTick(RENC* renc, uint16_t to);
uint8_t	RENC_buttonStatus(RENC* renc);				// The Encoder button current status: 0	- not pressed, 1 - short press, 2 - long press

#ifdef __cplusplus
}
#endif

struct _class_RENC {
    volatile uint32_t	button_data;                // Exponential average value of button port (to debounce)
    uint16_t          	over_press;                 // Maximum time the button can be pressed (ms)
    uint16_t			tick_period;            	// Repeat 'tick' period
    uint32_t            tick_time;					// Time in ms when the last 'tick' was generated
    volatile uint32_t 	bpt;                        // Time when the button was pressed (press time, ms)
    GPIO_TypeDef* 		b_port;						// The PORT of the press button
    uint16_t			b_pin;	    				// The PIN number of the button
    bool				i_b_rel;					// Ignore button release event
    bool				b_on;						// The button current position: true - pressed
    uint32_t			b_check;					// Time when the button should be checked (ms)
};

#ifdef __cplusplus

class BTN {
	public:
		BTN(GPIO_TypeDef* ButtonPORT, uint16_t ButtonPIN)	{ RENC_createButton(&renc, ButtonPORT, ButtonPIN);	}
		void		setTimeout(uint16_t timeout_ms)			{ RENC_setTimeout(&renc, timeout_ms);				}
		bool    	setTick(uint16_t to)					{ return RENC_setTick(&renc, to);					}
		uint8_t		buttonStatus(void)						{ return RENC_buttonStatus(&renc);					}
	private:
		RENC			renc;
};
#endif

#endif
