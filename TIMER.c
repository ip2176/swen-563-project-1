/*
  The timer file is used to encapsulate the functions that relate to using the TIM2 register
*/

#include "TIMER.h"

/*
  Helper function to take care of initializing the timer registers.
  Loads the prescaler value (forces the load), then enables capture
  input
*/
void timer_init(){

  // Enable the TIM2 clock
  TIMER_2 |= TIMER_2_ENABLE;

  // Adjust the TIM2 device
  TIMER_2_PRESCALER          = TIMER_2_PRESCLAER_RATIO;        // Load a prescale value to make the TIM2 clock rate match the GPIO clock rate
  TIMER_2_PRESCALER_ENABLE   |= TIMER_2_PRESCALER_FORCE_LOAD;  // Force the board to load the prescaler value and match the clock rates
  TIMER_2_CAPTURE_INPUT      &= CLEAR;                         // Clear the input register
  TIMER_2_CAPTURE_COMPARE    |= TIMER_2_CAPTURE_SET_TO_INPUT;  // Make sure the capture channel is set to input mode
  TIMER_2_CAPTURE_INPUT     |= TIMER_2_ENABLE_INPUT_CAPTURE;   // Enable capturing data on TIM2
}

/*
  Helper function to enable the capture bit on the TIM2 device so
  it starts capturing data
*/
void start_capture(){
  TIMER_2_CAPTURE |= TIMER_2_ENABLE_INPUT_CAPTURE;
}

/*
  Helper function to disable the capture bit on the TIM2 device so
  it stops capturing data
*/
void stop_capture(){
  TIMER_2_CAPTURE &= TIMER_2_DISABLE_INPUT_CAPTURE;
}

/*
  Helper function that returns the value of the current
  measurement in the TIM2 results register

  Output: An unsigned 32bit integer that represents the value
          in the register, this value is the millisecond
          representation of the current measurement of the time
*/
uint32_t get_current_measurement(){
  return (uint32_t)TIMER_2_CLOCK_TIMER_RESULTS;
}

/*
  Helper function that returns the value of the current
  measurement in the TIM2 clock count register

  Output: An unsigned 32bit integer that represents the value
          in the register, this value is the millisecond
          representation of the current measurement of the count
*/
uint32_t get_current_time(){
  return (uint32_t)TIMER_2_COUNT;
}

/*
  Helper function to determine if a measurement can be made.  Uses the 
  TIM2 status register to detect when a measurement is ready to read

  Output: Returns an integer (the value in the register) will be 1 or 0
*/
int measurement_detected(){
  return TIMER_2_RESULT_FOUND;
}
