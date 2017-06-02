#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <dirent.h>


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


    // variables to hold string from Client
    char clientCommand[500] = {};

    while(1){
      connection_fd = accept(sock_fd, (struct sockaddr *) &client_addr, &client);

      if(connection_fd < 0){
        fprintf(stderr, "error on accept\n");
        exit(1);
      }



      //printf("got a new connection from \n");


      // wait to receive client command, change from sock_fd to connection
      recv(connection_fd, clientCommand, 500,10);
   

      if(strlen(clientCommand) ==0){
        printf("got nothing from client\n");
      }

      // temp print out command to verify
      printf("The command is: %s\n", clientCommand);
      printf("The length of the command is %lu\n", strlen(clientCommand));

      // Tokenize the command so we get the -l -g, somefile,
      // there is no /n character sent from the client
      char * words[512];
      if(clientCommand != NULL){
        int i = 0; //variable to tell where to put tokenized string
        char *p = strtok(clientCommand, " ");
        while( p != NULL){
          // add string to word array and increment i
          words[i++] = p;
          p = strtok(NULL, " ");
        }
      }
 
      printf("The scond word is %s\n", words[1]);






      // File pointer
      FILE *file;
      char bufferForFile[100000];
      file = fopen("GitHubVS.sln", "r");
      fscanf(file, "%s", bufferForFile);
      int pp;
      char fileBuffer[512] = {};
      
      while((pp = fread(fileBuffer, sizeof(char), 512, file)) > 0){
        //printf("hello\n");
        if(send(connection_fd, fileBuffer, pp, 0) < 0){

          printf("error sending\n");
        } // end if
      }

  
      //int n = write(connection_fd, bufferForFile, 100000);
      // printf("The return val n is %d\n", n);

      //  write for -l all the files in this folder
      char bufferForFileNames[100000] = {};
      bufferForFileNames[0] = '\0';

      // get every file name in this folder and append
      DIR *dir;
      struct dirent *sd;

      DIR *d;
      struct dirent *dirr;
      d = opendir(".");
      if(d){
        while((sd = readdir(d)) != NULL){
          char fileName[1000];
          strcpy(fileName, sd->d_name);
          printf("file name is %s\n", fileName);
          // add each file name to the string
          strcat(bufferForFileNames, fileName);
          strcat(bufferForFileNames, "\n");
        }  // end while
      } // end if

      // write file names to the client
      //write(connection_fd, bufferForFileNames, strlen(bufferForFileNames));      

      closedir(d);

    } // end while loop
  } // end while loop

}  // end main
