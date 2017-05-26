#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* method prototypes */
void validateParameters(int, char* []);

/* Function to validate that user 
 * entered valid port number.
 * If valid port number is entered, 
 * function completes and returns to main,
 * else, usage is displayed to user
 *  */

void validateParameters(int argc, char *argv[]){

  // if command line does not have 2 arguments
  // show usage to usr
  if(argc != 2){
   printf("Usage example: ./ftserver <portNumber>\n");
   exit(1);
  }
}









int main(int argc, char * argv[]){

  printf("Hello\n");
  validateParameters(argc, argv);
  printf("After validate params\n");
}
