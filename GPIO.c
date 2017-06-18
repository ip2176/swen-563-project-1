/*
  Function definitions for the GPIO functions
*/

#include "GPIO.h"

/*
  Function to set the necessary bits in all of the GPIO registers to
  enable the PA0 pin, the GPIO clock, and tie the PAO pin to the TIM2
  timer
*/
void gpio_init(){
  // enable the peripheral clock of GPIO Port
  GPIO_CLOCK |= GPIO_CLOCK_ENABLE;

  // Connect the TIM2 timer to the GPIO Pin A0
  GPIO_A_PIN &= CLEAR;                                                        // Clear the register
  GPIO_A_PIN |= GPIO_ALTERNATE_FUNCTION_MODE;                                 // Put PA0 in alternate function mode
  GPIO_PA0_ALTERNATE_FUNCTION |= GPIO_PA0_ENABLE_ALTERNATE_FUNCTION_TIM2;     // Tie PA0 to to TIM2
}
