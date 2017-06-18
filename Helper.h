/*
  Header file for helper function definitions
*/

#include "USART_Helper.h"
#include "CONSTANTS.h"

#include <string.h>

/*
  Check the input string and see if we have a valid character in it.
  Used to check the user input

  Input: input - The string to check if it is in valid characters
         valid_characters - A string of valid characters input should
                            be checked against
  Output: An integer for success (1) if the input string contained any 
          valid characters or failure (0) if it didn't
*/
int check_for_valid_input(char *input, char *valid_characters);

/*
  Helper function to keep the board form doing anythiung else until
  reset is pushed
*/
void exit_program(void);
