#include "stm32l476xx.h"
#include "CONSTANTS.h"

void timer_init(){

	// enable our timer
	TIMER_2 |= TIMER_2_ENABLE;

	// configure timer
	TIMER_2_PRESCALER 				 = TIMER_2_PRESCLAER_RATIO; 	     // load prescale value (1MHz)
	TIMER_2_PRESCALER_ENABLE 	|= TIMER_2_PRESCALER_FORCE_LOAD;     // force load of new prescaler
	TIMER_2_CAPTURE_INPUT 	 	&= CLEAR; // turn off capture input until we're ready with updates
	TIMER_2_CAPTURE_COMPARE  	|= TIMER_2_CAPTURE_SET_TO_INPUT;	     // set compare/capture channel for input and clear filter
	TIMER_2_CAPTURE_INPUT 		|= TIMER_2_ENABLE_INPUT_CAPTURE;	     // enable capture input
}

void start_capture(){
	TIMER_2_CAPTURE |= TIMER_2_ENABLE_INPUT_CAPTURE;
}

void stop_capture(){
	TIMER_2_CAPTURE &= TIMER_2_DISABLE_INPUT_CAPTURE;
}

uint32_t get_current_measurement(){
	return (uint32_t)TIMER_2_CLOCK_TIMER_RESULTS;
}

uint32_t get_current_time(){
	return (uint32_t)TIMER_2_COUNT;
}

int measurement_detected(){
	return TIMER_2_RESULT_FOUND;
}
