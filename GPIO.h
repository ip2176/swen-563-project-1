/*
  Header file for the GPIO helper functions
*/

#include "stm32l476xx.h"
#include "CONSTANTS.h"

/*
  Function to set the necessary bits in all of the GPIO registers to
  enable the PA0 pin, the GPIO clock, and tie the PAO pin to the TIM2
  timer
*/
void gpio_init(void);
