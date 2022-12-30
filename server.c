#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

/* Defines */
#define SERVER_PORT 80
#define MAXLENGTH   90

int main() {
  int                 serverSocket, clientSocket;
  struct sockaddr_in  serverAddress, clientAddr;
  int                 status, bytesRcv;
  socklen_t           addrSize;
  char                buffer[30];
  char*               response = "OK";

  char input_csv[MAXLENGTH];
  FILE* csv_file;
  
  // Use a while loop to ask user for the name of the file containing the Pokemon descriptions
  printf("Enter the name of the file containing the Pokemon descriptions: \n");
  while (1) {
    if (strcmp(input_csv, "q") == 0)
      exit(EXIT_FAILURE);
    scanf("%s", input_csv);
    csv_file = fopen(input_csv, "r");
    if (!csv_file) {
      printf("Pokemon file is not found. Please enter the name of the file again or press 'q' to quit.\n");
      continue;
    } else {
      break;
    }
  }

  // Create the server socket
  serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (serverSocket < 0) {
    printf("*** SERVER ERROR: Could not open socket.\n");
    exit(-1);
  }

  // Setup the server address
  memset(&serverAddress, 0, sizeof(serverAddress)); // zeros the struct
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddress.sin_port = htons((unsigned short) SERVER_PORT);

  // Bind the server socket
  status = bind(serverSocket,  (struct sockaddr *)&serverAddress, sizeof(serverAddress));
  if (status < 0) {
    printf("*** SERVER ERROR: Could not bind socket.\n");
    exit(-1);
  }

  // Set up the line-up to handle up to 5 clients in line 
  status = listen(serverSocket, 5);
  if (status < 0) {
    printf("*** SERVER ERROR: Could not listen on socket.\n");
    exit(-1);
  }

  // Wait for clients now
  while (1) {
    addrSize = sizeof(clientAddr);
    clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddr, &addrSize);
    if (clientSocket < 0) {
      printf("*** SERVER ERROR: Could not accept incoming client connection.\n");
      exit(-1);
    }
    printf("SERVER: Received client connection.\n");

    // Go into infinite loop to talk to client
    while (1) {
      // Get the message from the client
      bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
      buffer[bytesRcv] = 0; // put a 0 at the end so we can display the string
      printf("SERVER: Received client request: %s\n", buffer);


      char line[MAXLENGTH];
      fseek(csv_file, 0, SEEK_SET); // Moves the file marker to beginning of the file.
  
      char  type1[MAXLENGTH];
      char  temp[MAXLENGTH];
      char* token;
      char  input[30];
      strcpy(input, buffer);
      // Read each line in the file, 
      // if the type1 matches the input, send this line to client
      while (fscanf(csv_file, "%[^\n]\n", line) != EOF) {
        stpcpy(temp, line);
        token = strtok(temp, ",");
        token = strtok(NULL, ",");
        token = strtok(NULL, ",");
        strcpy(type1, token);
        if (strcmp(type1, input) == 0) {
          send(clientSocket, line, strlen(line), 0);
        
          bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
          buffer[bytesRcv] = 0; // put a 0 at the end so we can display the string
          if (strcmp(buffer, "Received") == 0) // Receive an acknowledgement from the client after sending one line.
            continue;
        }
      }

      // When a search is complete, respond with an "OK" message.
      printf("SERVER: Sending \"%s\" to client\n", response);
      send(clientSocket, response, strlen(response), 0);
      if ((strcmp(buffer,"done") == 0) || (strcmp(buffer,"stop") == 0))
	      break;
    }
    printf("SERVER: Closing client connection.\n");
    close(clientSocket); // Close this client's socket

    // If the client said to stop, then I'll stop myself
    if (strcmp(buffer,"stop") == 0)
      break;
  }

  fclose(csv_file);

  // Close the socket
  close(serverSocket);
  printf("SERVER: Shutting down.\n");
}
