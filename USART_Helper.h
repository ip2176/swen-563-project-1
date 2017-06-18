/*
  Header file for the USART helper functions
*/

#include "UART.h"
#include "CONSTANTS.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/*
  Helper function to handle the usart write function syntax.  Automatically adds
  the newlines to the string so we don't have to do that later
  
  Input: This function takes a string (character array pointer)
*/
void usart_write_simple(char *message);

/*
  This function helps the user to see what they are typing into the console

  Input: data - a character that came from the USART_Read method
         print_newline - Switch used to tell the function to print a newline as well as the data
*/
void usart_real_time_write(char data, int print_newline);

/*
  Helper function to write out data values without having to worry
  about all the fiddly bits for int->string conversion.  Allows
  for the familiar sprintf format for accepting a variable number
  of arguments

  Input: This function takes a string (character array pointer) and 
         any number of format data numbers after that, such as
         function("Example string %d, %d", num1, num2)
*/
void usart_write_data_string(char *message, ...);

/*
  Helper function to handle the usart read function syntax

  Output: Returns the output of the USART_Read function
*/
char usart_read_simple(void);
