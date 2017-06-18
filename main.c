#include "SysClock.h"
#include "LED.h"
#include "TIMER.h"
#include "GPIO.h"
#include "Helper.h"

#include <stdlib.h>

uint16_t measurement_results[BUCKETS];  // Area for measurements
uint32_t initial_measurement = 0;        // For use with the POST functionality
int lower_bound = 950;                  // The lower bound default (in milliseconds)

// This define is kept here because its simpler for execution
#define UPPER_BOUND (lower_bound + 100)

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
  usart_real_time_write(user_input, PRINT_NEWLINE);

  // Make sure we have expected input
  while(!check_for_valid_input(&user_input, VALID_YES_NO)){
    usart_write_simple("Unrecognized input, please use Y or N");
    user_input = usart_read_simple();
    usart_real_time_write(user_input, PRINT_NEWLINE);
  }

  // If the user does not want to continue, break the loop
  if(check_for_valid_input(&user_input, VALID_NO)){
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
  usart_real_time_write(user_input, PRINT_NEWLINE);

  // Make sure the user said something intelligible
  while(!check_for_valid_input(&user_input, VALID_YES_NO)){

    // Handle if the user input a newline, makes the output look weird if they do
    if(user_input == ASCII_NEWLINE){
      usart_write_simple("Invalid input (\\n or \\r) please try again (Y or N)");
    }
    else {
      usart_write_data_string("Invalid input (%c) please try again (Y or N)", user_input);
    }

    // Keep asking
    usart_write_data_string(change_user_input_message, lower_bound);
    user_input = usart_read_simple();

    // Print out what they put in
    usart_real_time_write(user_input, PRINT_NEWLINE);
  }

  // This is only if the user wants to set a new value, otherwise it defaults to 950
  if(check_for_valid_input(&user_input, VALID_YES)){
    int data_valid = 0;
    char data_input;
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

        usart_real_time_write(data_input, NO_NEWLINE);

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
  This function actually measures the timing of each of the pulsesd and puts
  the data into the results array
*/
void take_measurements(){
  int number_of_measurements = 0;
  int first_measurement = 1;
  uint32_t previous_measurement, current_measurement, difference_measurement = 0;
  usart_write_simple("Taking measurements...");

  start_capture();

  // Start capturing data while we have less than 1000 measurements
  while(number_of_measurements <= MEASUREMENTS){

    // We got a measurement!
    if(measurement_detected()){

      // We need to get two measurements to get a data value
      if(first_measurement){
        previous_measurement = get_current_measurement();
        first_measurement = 0;
      }

      // We have two measurements to get a difference
      else {
        current_measurement = get_current_measurement();
        difference_measurement = current_measurement - previous_measurement;
        previous_measurement = current_measurement;

        // Record the measurement if its within bounds
        if(difference_measurement <= UPPER_BOUND && difference_measurement >= lower_bound){
          measurement_results[difference_measurement - lower_bound]++;
        }

        // Increment the number of measurements we took.  This means the output wont always be 1000, but it
        // saves us from an infitie loop
        number_of_measurements++;
      }
    }
  }
  stop_capture();
}

/*
  This function takes the results in the results array and prints them to the USART
  window
*/
void print_results(){

  // Use a copy because we don't want to increment it.  It will throw off the print
  // if the user decides to continue
  int lower_bound_copy = lower_bound;

  usart_write_simple(DASHES);
  
	usart_write_data_string("Lower bound: %d ms", lower_bound);
	usart_write_data_string("Upper bound: %d ms", UPPER_BOUND);
	usart_write_simple("Results from testing:");
	usart_write_simple(DASHES);

  // Print out each result that we actually have, skip those we dont
  for(int x = 0; x < BUCKETS; x++){

    // Increment the lower bound up, use this in the histogram data
    lower_bound_copy++;

    // Check if the measurement was not zero
    if(measurement_results[x] != NO_RESULT){

      // Print our histogram data finally :)
      usart_write_data_string("%4d ms: %d measurements", lower_bound_copy, measurement_results[x]);
    }
  }
  usart_write_simple(DASHES);
}

/*
  Function to handle checking if the user wants to continue testing or not
*/
void check_for_continuation() {
  char user_input;
  usart_write_simple("Take more measurements? (Y or N)");

  user_input = usart_read_simple();

  usart_real_time_write(user_input, PRINT_NEWLINE);

  // If the user does not want to continue, break the loop
  if(check_for_valid_input(&user_input, VALID_NO)){
    exit_program();
  }
}

/*
  Clear out the measurement results
*/
void clear_results(){
  memset(measurement_results, 0, sizeof(measurement_results));
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
