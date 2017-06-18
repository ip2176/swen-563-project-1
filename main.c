#include "stm32l476xx.h"
#include "SysClock.h"
#include "LED.h"
#include "UART.h"
#include "TIMER.h"
#include "CONSTANTS.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

uint16_t measurement_results[BUCKETS];  // Area for measurements
uint32_t initial_measurement = 0;  			// For use with the POST functionality
int lower_bound = 950;									// The lower bound default (in milliseconds)

// This define is kept here because its simpler for execution
#define UPPER_BOUND (lower_bound + 100)

/*
	Helper function to handle the usart write function syntax.  Automatically adds
	the newlines to the string so we don't have to do that later
	
	Input: This function takes a string (character array pointer)
*/
void usart_write_simple(char *message){
	char buffer[strlen(message) + strlen(CARRIAGE_RETURN_NEWLINE)];
	strcpy(buffer, message);
	strcat(buffer, CARRIAGE_RETURN_NEWLINE);
	USART_Write(USART2, (uint8_t *)buffer, strlen(buffer));
}

/*
	Helper function to write out data values without having to worry
	about all the fiddly bits for int->string conversion.  Allows
	for the familiar sprintf format for accepting a variable number
	of arguments

	Input: This function takes a string (character array pointer) and 
				 any number of format data numbers after that, such as
				 function("Example string %d, %d", num1, num2)
*/
void usart_write_data_string(char *message, ...){

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

	Output: Returns the output of the USART_Read function
*/
char usart_read_simple(){
	return USART_Read(USART2);
}

/*
	Check the input string and see if we have a valid character in it.
	Used to check the user input

	Input: input - The string to check if it is in valid characters
				 valid_characters - A string of valid characters input should
                            be checked against
  Output: An integer for success (1) if the input string contained any 
					valid characters or failure (0) if it didn't
*/
int check_for_valid_input(char *input, char *valid_characters){
	if(!strpbrk(input, valid_characters)){
		return FAILURE;
	}
	return SUCCESS;
}

/*
	Helper function to keep the board form doing anythiung else until
	reset is pushed
*/
void exit_program(){
	usart_write_simple("Exiting the program");
	while(1);
}

/*
	Function to set the necessary bits in all of the GPIO registers to
	enable the PA0 pin, the GPIO clock, and tie the PAO pin to the TIM2
	timer
*/
void gpio_init(){
	// enable the peripheral clock of GPIO Port
	GPIO_CLOCK |= GPIO_CLOCK_ENABLE;

	// Connect the TIM2 timer to the GPIO Pin A0
	GPIO_A_PIN &= CLEAR; 															 												// Clear the register
	GPIO_A_PIN |= GPIO_ALTERNATE_FUNCTION_MODE;	       												// Put PA0 in alternate function mode
	GPIO_PA0_ALTERNATE_FUNCTION |= GPIO_PA0_ENABLE_ALTERNATE_FUNCTION_TIM2;		// Tie PA0 to to TIM2
}

/*
	This function handles getting the user input when the post fails and
	allows the user to tell the program if they wish to try again

	Output: Returns a uint32 corresponding to the current measurement from the timer.
					Used to refresh the 'initial' measurement and make sure we have at least
					one event in 100 ms
*/
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

/*
	Post functionality checks if the board is receiving any signals on
	the GPIO PA0 pin (at leats 1 in 100 ms.  Automatically handles failure
	conditions by calling the post failed funtion
*/
void post(){
	int post_success = 0;
	uint32_t current_measurement = 0;

	usart_write_simple("Starting POST...");

	// Start capturing so we can see if we got any rising edges
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

			// We didn't get a pulse, fail the post, ask the use rif they want to continue
			else{
				initial_measurement = post_failed();
			}
		}

		// We did not detect a measurement yet, check for timeout
		else {
			current_measurement = get_current_time();

			// Timeout check
			if(current_measurement - initial_measurement > ONE_HUNDRED_MILLISECONDS){
				initial_measurement = post_failed();
			}
		}
	}

	// Stop capturing when we have success
	stop_capture();
}

/* 
	This function actually measures the timing of each of the pulsesd and puts
	the data into the results array
*/
void take_measurements(){
}

/*
	This function takes the results in the results array and prints them to the USART
	window
*/
void print_results(){
}

/*
	This function handles setting the upper and lower bounds for measuring pulses.
	The upper bound ius set implicitly int he # define, while the lower bound is set
	explicitly here
*/
void get_user_input(){
	char user_input;
	char *change_user_input_message = "Would you like to change the current lower bound (%d)? (Y or N)";
	char *enter_lower_bound_message = "Enter a new lower bound (%d-%d)";

	// Start gathering user input
	usart_write_data_string(change_user_input_message, lower_bound);
	user_input = usart_read_simple();

	// Make sure the user said something intelligible
	while(!check_for_valid_input(&user_input, VALID_YES_NO)){

		// Handle if the user input a newline, makes the output look weird if they do
		if(user_input == ASCII_NEWLINE){
			usart_write_simple("Invalid input (\\n or \\r) please try again");
		}
		else {
			usart_write_data_string("Invalid input (%c) please try again", user_input);
		}
		
		// Keep asking
		usart_write_data_string(change_user_input_message, lower_bound);
		user_input = usart_read_simple();
	}

	// This is only if the user wants to set a new value, otherwise it defaults to 950
	if(check_for_valid_input(&user_input, "Yy")){
		
		int data_valid, data_start = 0;
		char data_input;
		char write_buffer[REAL_TIME_BUFFER_SIZE] = {NULL};
		char lower_bound_buffer[LOWER_BOUND_BUFFER_SIZE + 1] = {NULL, NULL, NULL, NULL, NULL};

		// Start capturing data
		usart_write_data_string(enter_lower_bound_message, LOWEST_LOWER_BOUND, HIGHEST_LOWER_BOUND);
		data_input = usart_read_simple();

		while(!data_valid){

			int input_number = 0;

			// Loop while the user hasn't hit enter and while we have space left in the
			// buffer
			while((data_input != ASCII_NEWLINE && input_number < LOWER_BOUND_BUFFER_SIZE)){

				lower_bound_buffer[input_number] = data_input;

				// Copy the data into a real time buffer for USART output
				write_buffer[data_start] = data_input;

				// Use a regular write here so we don't get newlines, let the user know
				// what they are typing
				USART_Write(USART2, (uint8_t *)write_buffer, sizeof(write_buffer));

				// Reset the write buffer
				memset(write_buffer, NULL, sizeof(write_buffer));

				// Move on to the next character
				input_number++;

				// Read again
				data_input = usart_read_simple();
			}

			// Check if the given number is valid
			lower_bound = atoi(lower_bound_buffer);
			if(lower_bound > HIGHEST_LOWER_BOUND || lower_bound < LOWEST_LOWER_BOUND){

				// Reset the array
				memset(lower_bound_buffer, NULL, sizeof(lower_bound_buffer));

				usart_write_simple(""); // Print a newline
				usart_write_data_string("Invalid lower bound (%d)", lower_bound);
				usart_write_data_string(enter_lower_bound_message, LOWEST_LOWER_BOUND, HIGHEST_LOWER_BOUND);
				data_input = usart_read_simple();
			}

			// We got a valid number
			else {
				data_valid = 1;
			}
		}
		usart_write_simple(""); // Print a newline
		usart_write_data_string("New lower bound: %d", lower_bound);
	}
}

/*
	Helper function to clear out the measurement results
*/
void clear_results(){
  for(int i = 0; i < BUCKETS; i++){
    measurement_results[i] = 0;
  }
}

/*
	Function to handle checking if the user wants to continue testing or not
*/
void check_for_continuation() {
  char user_input;
  usart_write_simple("Take more measurements?");

  user_input = usart_read_simple();
  
  // If the user does not want to continue, break the loop
  if (user_input == 'N' || user_input == 'n'){
    exit_program();
  }
}

/*
	The main function initializes all of the parts of the board needed for testing,
	then attempts a post.  After the post is successful, the function
	gets the bounds from the user.  The function then takes measurements and builds
	a histograsm to display to the user.  If the user wants to continue again, the
	function clears the results, then gets new bounds and repeats the measurement
	process until the user decides to exit
*/
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

    // Ask user if they want to go again
    check_for_continuation();

    // If the user didn't break out, clear the results for the next round
    clear_results();
  }
}
