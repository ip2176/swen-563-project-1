/*
  This file contains all fo the constant definitions used for this project.  They are
  consolidated for easy reading and access
*/

// General definitons used in main.c
#define MEASUREMENTS (1000)                              // Take one thousand measurments
#define BUCKETS (101)                                    // One bucket for each millisecond measurement on histograsm
#define ONE_HUNDRED_MILLISECONDS  (100000)               // Used during the post routine to detect if the board is at least seeing one pulse
#define OUTPUT_BUFFER_SIZE (1000)                        // Arbitrary bad code, I know
#define SUCCESS (1)                                      // Used for some int returning functions
#define FAILURE (0)                                      // Used for some int returning functions
#define NO_RESULT (0)                                    // Used in histogram printing
#define VALID_YES_NO ("YyNn")                            // Used for input checking
#define VALID_YES  ("Yy")                                // Used to check if the user said yes to a prompt
#define VALID_NO  ("Nn")                                 // Used to check if the user said yes to a prompt
#define LOWEST_LOWER_BOUND (50)                          // Used for input checking the lower bound
#define HIGHEST_LOWER_BOUND (9950)                       // Used for input checking the lower bound
#define LOWER_BOUND_BUFFER_SIZE (4)                      // Used for setting the lower bound buffer size
#define ASCII_NEWLINE  (13)                              // Ugh, have to check for newlines
#define PRINT_NEWLINE (1)                                // Tells the real time printer to print a newline
#define NO_NEWLINE (0)                                   // Tellks the real time printer not to print a newline
#define REAL_TIME_BUFFER_SIZE (1)                        // Used to output the users input in real time
#define REAL_TIME_BUFFER_START (0)                       // Used for printing out real time data as its entered in
#define CARRIAGE_RETURN_NEWLINE ("\r\n")                 // Used in strings in the program
#define DASHES ("--------------------------------------------------------------------------------") // Used to make printing look nice

// General use 
#define CLEAR   (~(0xFFFFFFFF))                          // Constant to clear a 32 bit register
#define ENABLE  (0x1)                                    // Enable constant
#define DISABLE (0x0)                                    // Disable constant

// Defines for the TIM2 timer to make the constants more human readbale
#define TIMER_2_ENABLE (RCC_APB1ENR1_TIM2EN)             // This allow us to enable the TIM2 timer
#define TIMER_2 (RCC->APB1ENR1)                          // The timer 2 register
#define TIMER_2_PRESCALER (TIM2->PSC)                    // The timer 2 presclater 
#define TIMER_2_PRESCALER_ENABLE (TIM2->EGR)             // Memory are for enabling the prescaler for TIM2
#define TIMER_2_PRESCALER_FORCE_LOAD (TIM_EGR_UG)        // Force the reload of the precaler value
#define TIMER_2_CAPTURE_INPUT (TIM2->CCER)               // The TIM2 input memory area
#define TIMER_2_CAPTURE_COMPARE (TIM2->CCMR1)            // The TIM2 capture/compare memory area
#define TIMER_2_CAPTURE  (TIM2->CR1)                     // The memory area to enable capturing
#define TIMER_2_CLOCK_TIMER_RESULTS (TIM2->CCR1)         // The TIM2 clock (where we get the measurement results)
#define TIMER_2_COUNT (TIM2->CNT)                        // The TIM2 count
#define TIMER_2_CAPTURE_SET_TO_INPUT (ENABLE)            // Set capture compare to input
#define TIMER_2_ENABLE_INPUT_CAPTURE (ENABLE)            // Enable input capture
#define TIMER_2_DISABLE_INPUT_CAPTURE (DISABLE)          // Disable input capture
#define TIMER_2_PRESCLAER_RATIO (80)                     // The TIM2 timer starts in 80Mhz so difive it by 80 so we match the GPIO clock rate of 1Mhz
#define TIMER_2_STATUS_REGISTER (TIM2->SR)               // The status register for timer 2
#define TIMER_2_RESULT_FOUND (TIMER_2_STATUS_REGISTER & 0x2)  // We found a result (rising edge)

// Defines for the GPIO to make the constants more human readbale
#define GPIO_CLOCK_ENABLE (RCC_AHB2ENR_GPIOAEN)          // This allows us to enable the GPIO timer
#define GPIO_CLOCK (RCC->AHB2ENR)                        // The GPIO clock
#define GPIO_A_PIN (GPIOA->MODER)                        // The GPIO A pin
#define GPIO_ALTERNATE_FUNCTION_MODE (0x2)               // For GPIO A 0 pin, this allows us to enable in alternate function mode
#define GPIO_PA0_ALTERNATE_FUNCTION (GPIOA->AFR[0])      // GPIO PIN A 0 alternate function
#define GPIO_PA0_ENABLE_ALTERNATE_FUNCTION_TIM2 (ENABLE) // Connects GPIOA to TIM2

