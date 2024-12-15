#include "button.h"
#include "tools.h"

#define	RENC_bounce      	50           			/* Bouncing timeout (ms) */
#define RENC_def_over_press	2500					/* Default value for button overpress timeout (ms) */
#define RENC_trigger_on		100						/* avg limit to change button status to on */
#define RENC_trigger_off 	50						/* avg limit to change button status to off */
#define RENC_avg_length   	4						/* avg length */
#define RENC_b_check_period	20						/* The button check period, ms */
#define RENC_long_press		1500					/* If the button was pressed more that this timeout, we assume the long button press */

static int32_t RENC_empAverage(RENC* renc, int32_t value) {
	uint8_t round_v = RENC_avg_length >> 1;
	renc->button_data += value - (renc->button_data + round_v) / RENC_avg_length;
	return (renc->button_data + round_v) / RENC_avg_length;
}

void RENC_createButton(RENC* renc, GPIO_TypeDef* ButtonPORT, uint16_t ButtonPIN) {
	renc->bpt 			= 0;
	renc->b_port 		= ButtonPORT;
	renc->b_pin  		= ButtonPIN;
	renc->over_press	= RENC_def_over_press;
	renc->tick_period	= 0;
	renc->tick_time		= 0;
	renc->i_b_rel		= false;
	renc->b_on			= false;
	renc->b_check		= 0;
}

void RENC_setTimeout(RENC* renc, uint16_t timeout_ms) {
	renc->over_press	= timeout_ms;
	renc->tick_period	= 0;
}

bool RENC_setTick(RENC* renc, uint16_t to) {
    if (to > 0) {                                               // Setup tick period
        renc->tick_period = constrain(to, 100, 900);
        renc->over_press  = 16000;
        return true;
    } else {                                                    // disable tick
        renc->tick_period = 0;
        renc->over_press  = RENC_def_over_press;
    }
    return false;
}

static uint8_t RENC_buttonTick(RENC* renc) {
    uint32_t now_t = HAL_GetTick();
    if (now_t - renc->tick_time > renc->tick_period) {
        renc->tick_time = now_t;
        return (renc->bpt != 0);
    }
    return 0;
}

/*
 * The Encoder button current status
 * 0	- not pressed
 * 1	- short press
 * 2	- long press
 */
uint8_t	RENC_buttonStatus(RENC *renc) {
	if (HAL_GetTick() >= renc->b_check) {			// It is time to check the button status
		renc->b_check = HAL_GetTick() + RENC_b_check_period;
		uint8_t s = 0;
		if (GPIO_PIN_SET == HAL_GPIO_ReadPin(renc->b_port, renc->b_pin))	// if port state is high, the button pressed
			s = RENC_trigger_on << 1;
		if (renc->b_on) {
			if (RENC_empAverage(renc, s) < RENC_trigger_off)
				renc->b_on = false;
		} else {
			if (RENC_empAverage(renc, s) > RENC_trigger_on)
				renc->b_on = true;
		}

		if (renc->b_on) {                           // Button status is 'pressed'
			uint32_t n = HAL_GetTick() - renc->bpt;
			if ((renc->bpt == 0) || (n > renc->over_press)) {
				renc->bpt = HAL_GetTick();
			} else if (n > RENC_long_press) {     	// Long press
				if (renc->i_b_rel) {
					return 0;
				} else {
					if (renc->tick_period)
						return RENC_buttonTick(renc);
					renc->i_b_rel = true;
					return 2;
				}
			}
		} else {                                    // Button status is 'not pressed'
			if (renc->bpt == 0 || renc->i_b_rel) {
				renc->bpt = 0;
				renc->i_b_rel = false;
				return 0;
			}
			uint32_t e = HAL_GetTick() - renc->bpt;
			renc->bpt = 0;							// Ready for next press
			if (e < renc->over_press) {				// Long press already managed
				return 1;
			}
		}
	}
    return 0;
}
