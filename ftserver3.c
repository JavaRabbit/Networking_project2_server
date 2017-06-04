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
#include <sys/stat.h>

/* method prototypes */
void validateParameters(int, char* []);
void sendFile(char *[], int, char* []);
void showFiles(int, char* []);


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

  validateParameters(argc, argv);

  // file descriptors to be used
  int sock_fd, connection_fd, datasock_fd, dataConnection_fd, client, dataClient;

  // struct to hold IP address and port numbers
  struct sockaddr_in server, dataServer,  client_addr, dataClient_addr;
  int portNumber, dataPortNumber;

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
    
    printf("\nAt top of while loop B\n");
    connection_fd = accept(sock_fd, (struct sockaddr *) &client_addr, &client);

    if(connection_fd < 0){
      fprintf(stderr, "error on accept\n");
      exit(1);
    }

    // wait to receive client command, change from sock_fd to connection
    recv(connection_fd, clientCommand, 500,10);

    if(strlen(clientCommand) ==0){
      printf("got nothing from client\n");
    }


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
    } // end if

    //  Here the command is tokenized

    // If user wanted -g, check if File exists first  
    // since we need to use the first socket to 
    // tell the user that file does not exist
    if(strcmp("-g", words[3]) == 0){
      FILE *file;
      // if the file exists, go to sendFile()
      // otherwise, tell client file does not exist
      file = fopen(words[4], "r");
      if(file){
        // if file exists, send the file size to client
        
        struct stat st;
        stat(words[4], &st);
        int fileSize = st.st_size;
        write(connection_fd, &fileSize, 4);

        // receive ok from the client
        char ok[24] = {};
        recv(connection_fd, ok, 24, 0);
        //  call sendFile function to start sending file
        sendFile(words, argc, argv);
        continue; 
      } else {
        int notFound = -5;
        write(connection_fd, &notFound, 4);
      }
    }  else {
     // else user entered -l
     showFiles(argc, argv);

    }
    
  } // end while
}  // end main


void sendFile(char *words[], int argc,char *argv[]){ 
    
      // File pointer
      FILE *file;
      if( !(file = fopen(words[4], "r"))){
        printf("Problem opening file\n");
        return;
      }

      // variables for data socket
      int datasock_fd, dataConnection_fd, dataClient;
      struct sockaddr_in dataServer, dataClient_addr;
      int dataPortNumber;



      // If the file exists, send to the client
      int pp;
      char fileBuffer[1024] = {};
     
        // write the data on a separate port
        // the port will be words[5], remember that we need to 
        // change string to int
        // Basically, listen for the data connection
        //
        if((datasock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
          perror("data socket error\n");
        }
        // clear socket
        bzero(&dataServer, sizeof(dataServer));
        dataServer.sin_family = AF_INET;
        dataPortNumber = atoi(argv[5]);
        dataServer.sin_port = htons(9092);     //// fix!
        dataServer.sin_addr.s_addr = htons(INADDR_ANY);
        bind(datasock_fd, (struct sockaddr *) &dataServer, sizeof(dataServer));
        listen(datasock_fd, 5);
        printf("The data port number is %d\n", dataPortNumber); 
        printf("\nNow waiting for data connection...\n");
        dataConnection_fd = accept(datasock_fd, (struct sockaddr *) &dataClient_addr, &dataClient);
     
        while((pp = fread(fileBuffer, sizeof(char), 1024, file)) > 0){
          if(send(dataConnection_fd, fileBuffer, pp, 0) < 0){

            printf("error sending\n");
          } // end if
        }
        close(dataConnection_fd);
        return;
}  // end of sendFile()



void showFiles(int argc, char *argv[]){

      // variables for data socket
      int datasock_fd, dataConnection_fd, dataClient;
      struct sockaddr_in dataServer, dataClient_addr;
      int dataPortNumber;

      if((datasock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("datasocket error\n");
      }
      bzero(&dataServer, sizeof(dataServer));
      dataServer.sin_family = AF_INET;
      dataPortNumber = atoi(argv[4]);  // 4 for -l
      dataServer.sin_port = htons(9092); ///FIXXXXXXX
      dataServer.sin_addr.s_addr = htons(INADDR_ANY);
      bind(datasock_fd, (struct sockaddr *) &dataServer, sizeof(dataServer));
      listen(datasock_fd, 5);
      printf("waiting for data connection for -l ...\n");
      dataConnection_fd = accept(datasock_fd, (struct sockaddr *) &dataClient_addr, &dataClient);

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
      write(dataConnection_fd, bufferForFileNames, strlen(bufferForFileNames));      

      closedir(d);


}  // end showFiles


