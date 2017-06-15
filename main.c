#include "stm32l476xx.h"
#include "SysClock.h"
#include "LED.h"
#include "UART.h"
#include "TIMER.h"  // Put timer capture stuff in here?

#include <string.h>
#include <stdio.h>

#define MEASUREMENTS 1000      // Take one thousand measurments
#define BUCKETS 101            // One bucket for each millisecon measurement on histograsm

uint8_t output_buffer[200];             // Output buffer array
uint16_t measurement_results[BUCKETS];  // Area for measurements

void POST(){
}

void POST_failed(){
}

void take_measurements(){
}

void print_results(){
}

void get_user_input(){
}

void clear_results(){

  // Clear out the measurement results
  for(int i = 0; i < BUCKETS; i++){
    measurement_results[i] = 0;
  }
}

char check_for_continuation() {
  char user_input;
  USART_Write(USART2, (uint8_t *)"Take more measurements?\r\n", 25);

  user_input = USART_Read(USART2);
  
  // If the user does not want to continue, break the loop
  if (user_input == 'N' || user_input == 'n'){
    USART_Write(USART2, (uint8_t *)"Exit!!!\r\n", 9);
  }
  return user_input;
}

int main(void){

  char user_input;

  // Initialize!
  System_Clock_Init();
  LED_Init();
  UART2_Init();

  /* Timer stuff ?
  RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;  //enable?
  // configure timer
  TIM2->PSC ????
  TIM2->EGR |= TIM_EGR_UG;
  TIM2->CCER ???
  TIM2->CCMR1 |= 0x1;  //enable?
  TIM2->CCER |= 0x1;   //enable?
  */

  // Make sure our hardware config is correct
  POST();

  while(1){

    // Get information from the user
    get_user_input();

    // Measure rising edges
    take_measurements();

    // Print out the results of every measurement
    print_results();

    // ask user if they want to go again
    user_input = check_for_continuation();

    // If the user wants to exit, break the loop
    if (user_input == 'N' || user_input == 'n'){
      break;
    }

    // If the user didn't break out, clear the results for the next round
    clear_results();
  }
}
