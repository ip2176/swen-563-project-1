/*
  This file defines helper print functions
*/

#include "USART_Helper.h"

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
  This function helps the user to see what they are typing into the console

  Input: data - a character that came from the USART_Read method
         print_newline - Switch used to tell the function to print a newline as well as the data
*/
void usart_real_time_write(char data, int print_newline){
  char write_buffer[REAL_TIME_BUFFER_SIZE] = {NULL};

  // Copy in the data
  write_buffer[REAL_TIME_BUFFER_START] = data;
  
  // Use a regular write here so we don't get newlines, let the user know
  // what they are typing
  USART_Write(USART2, (uint8_t *)write_buffer, sizeof(write_buffer));

  // Write a newline
  if(print_newline){
    usart_write_simple("");
  }
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
