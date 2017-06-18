/*
  Function declarations for the TIM2 functions
*/

#include "stm32l476xx.h"
#include "CONSTANTS.h"

/*
  Helper function to take care of initializing the timer registers.
  Loads the prescaler value (forces the load), then enables capture
  input
*/
void timer_init(void);

/*
  Helper function to enable the capture bit on the TIM2 device so
  it starts capturing data
*/
void start_capture(void);

/*
  Helper function to disable the capture bit on the TIM2 device so
  it stops capturing data
*/
void stop_capture(void);

/*
  Helper function that returns the value of the current
  measurement in the TIM2 results register

  Output: An unsigned 32bit integer that represents the value
          in the register, this value is the millisecond
          representation of the current measurement of the time
*/
uint32_t get_current_measurement(void);

/*
  Helper function that returns the value of the current
  measurement in the TIM2 clock count register

  Output: An unsigned 32bit integer that represents the value
          in the register, this value is the millisecond
          representation of the current measurement of the count
*/
uint32_t get_current_time(void);

/*
  Helper function to determine if a measurement can be made.  Uses the 
  TIM2 status register to detect when a measurement is ready to read

  Output: Returns an integer (the value in the register) will be 1 or 0
*/
int measurement_detected(void);
