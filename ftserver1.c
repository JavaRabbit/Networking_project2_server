#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>



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

  // file descriptors to be used
  int sock_fd, connection_fd, client;

  // struct to hold IP address and port numbers
  struct sockaddr_in server, client_addr;
  int portNumber;

  // assign port number to argv[1]
  portNumber = atoi(argv[1]);

  // Call to Socket function
  if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    perror("socket: ");
  }

  // clear socket
  bzero(&server, sizeof(server));

  // set addressing scheme to AF_INET
  server.sin_family = AF_INET;

  // Listen of specified port
  server.sin_port = htons(portNumber);

  // Allow any IP to connect
  server.sin_addr.s_addr = htons(INADDR_ANY);

  // Bind the host address
  bind(sock_fd, (struct sockaddr *) &server, sizeof(server));
 
  while(1){
    // start to listen, 
    listen(sock_fd,5);
    printf("Listening for a new connection...\n");

    client = sizeof(client_addr);

    while(1){
      connection_fd = accept(sock_fd, (struct sockaddr *) &client_addr, &client);

      if(connection_fd < 0){
        fprintf(stderr, "error on accept\n");
        exit(1);
      }
    } // end while loop
  } // end while loop

}  // end main
