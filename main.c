#include "stm32l476xx.h"
#include "SysClock.h"
#include "LED.h"
#include "UART.h"
#include "TIMER.h"
#include "CONSTANTS.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

uint16_t measurement_results[BUCKETS];  // Area for measurements
uint32_t initial_measurement = 0;  			// For use with the POST functionality

/*
	Helper function to handle the usart write function syntax
*/
void usart_write_simple(char *message){
	char buffer[strlen(message) + 4];
	strcpy(buffer, message);
	strcat(buffer, "\r\n");
	USART_Write(USART2, (uint8_t *)buffer, strlen(buffer));
}

/*
	Helper function to write out data values without having to worry
	about all the fiddly bits for int->string conversion
*/
void write_data_string(char *message, ...){

	// Use a variable size call to the function
	va_list data_points;
	va_start(data_points, message);

	// I know this is bad form but I have no idea how to make a variable size function
	// and compute a variable size list.  I used this just to make my life easier.  It works
	char buffer[OUTPUT_BUFFER_SIZE];
	vsprintf(buffer, message, data_points);
	va_end(data_points);
	usart_write_simple(buffer);
}

/*
	Helper function to handle the usart read function syntax
*/
char usart_read_simple(){
	return USART_Read(USART2);
}

/*
	Helper function to keep the board form doing anythiung else until
	reset is pushed
*/
void exit_program(){
	usart_write_simple("Exiting the program");
	while(1);
}

void gpio_init(){
	// enable the peripheral clock of GPIO Port
	GPIO_CLOCK |= GPIO_CLOCK_ENABLE;

	// Connect the TIM2 timer to the GPIO Pin A0
	GPIO_A_PIN &= CLEAR; 															 												// Clear the register
	GPIO_A_PIN |= GPIO_ALTERNATE_FUNCTION_MODE;	       												// Put PA0 in alternate function mode
	GPIO_PA0_ALTERNATE_FUNCTION |= GPIO_PA0_ENABLE_ALTERNATE_FUNCTION_TIM2;		// Tie PA0 to to TIM2
}

uint32_t post_failed(){
	char user_input;
	usart_write_simple("POST failed");
  usart_write_simple("Try again? (Y or N)");

	// Read the user input
  user_input = usart_read_simple();

	// Make sure we have expected input
	while(user_input != 'Y' && user_input != 'y' && user_input != 'N' && user_input != 'n'){
		usart_write_simple("Unrecognized input, please use Y or N");
		user_input = usart_read_simple();
	}

  // If the user does not want to continue, break the loop
  if (user_input == 'N' || user_input == 'n'){
		exit_program();
  }

	// This means we want to continue
	uint32_t current_measurement = get_current_measurement();
	return current_measurement;
}

void post(){
	int post_success = 0;
	uint32_t current_measurement = 0;

	usart_write_simple("Starting POST...");

	start_capture();
	initial_measurement = get_current_measurement();

	while(!post_success){
		if(measurement_detected()){
			current_measurement = get_current_measurement();
			
			// Check if we got at least one pulse in 100 milliseconds
			if(current_measurement - initial_measurement <= ONE_HUNDRED_MILLISECONDS){
				post_success = 1;
				usart_write_simple("POST complete");
			}

			else{
				initial_measurement = post_failed();
			}
		}
		else {
			current_measurement = get_current_time();

			// Timeout check
			if(current_measurement - initial_measurement > ONE_HUNDRED_MILLISECONDS){
				initial_measurement = post_failed();
			}
		}
	}
	stop_capture();
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
  usart_write_simple("Take more measurements?");

  user_input = usart_read_simple();
  
  // If the user does not want to continue, break the loop
  if (user_input == 'N' || user_input == 'n'){
    exit_program();
  }
  return user_input;
}

int main(void){

  // Initialize!
  System_Clock_Init();
  LED_Init();
  UART2_Init();
	gpio_init();
	timer_init();

  // Make sure our hardware config is correct
  post();

  while(1){

    // Get information from the user
    get_user_input();

    // Measure rising edges
    take_measurements();

    // Print out the results of every measurement
    print_results();

    // ask user if they want to go again
    check_for_continuation();

    // If the user didn't break out, clear the results for the next round
    clear_results();
  }
}
