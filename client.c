#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>

#include "dataProcess.h"
 
int main() {
  int                 clientSocket;
  struct sockaddr_in  clientAddress;
  int                 status, bytesRcv;

  pthread_t           t1;

  char                inStr[90];    // stores user input from keyboard
  char                buffer[90];   // stores sent and received data

  // Create socket
  clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (clientSocket < 0) {
    printf("*** CLIENT ERROR: Could open socket.\n");
    exit(-1);
  }

  // Setup address
  memset(&clientAddress, 0, sizeof(clientAddress));
  clientAddress.sin_family = AF_INET;
  clientAddress.sin_addr.s_addr = inet_addr(SERVER_IP);
  clientAddress.sin_port = htons((unsigned short) SERVER_PORT);

  // Connect to server
  status = connect(clientSocket, (struct sockaddr *) &clientAddress, sizeof(clientAddress));
  if (status < 0) {
    printf("Unable to establish connection to the PPS!\n");
    exit(-1);
  }

  //Allocate memory for Data struct that will be passed to thread functions
  Data *d1 = (Data *) malloc(sizeof(Data));
  if (d1 == NULL) {
    printf("Error: malloc\n");
    exit(EXIT_FAILURE);
  }

  d1->count = 0;
  d1->pokemons = NULL;
  d1->files = (char *) malloc(1000 *sizeof(char));
  if (d1->files == NULL) {
    printf("Error: malloc\n");
    exit(EXIT_FAILURE);
  }

  // Initialize the semaphore
  if (sem_init(&d1->mutex, 0, 1) < 0) {
    printf("Error: on semaphore init.\n");
    exit(EXIT_FAILURE);
  }

  // Always prompts a menu for the user to choose to search for files, save the search, or exit the program,
  // if the user choose a, the main thread will search for type1 provided by the user,
  // if the user choose b, another thread will be created to save the search pokemons (only completed search will be saved),
  // if the user choose c, the total number of queries completed and file names used to save search will be displayed, 
  // then program will be terminated.
  char option[MAXLENGTH];
  while (1) {
    printf("Enter the letter of your choice to continue (enter a or b or c): \n");
    printf("a. Type search\n");
    printf("b. Save results\n");
    printf("c. Exit the program\n");
    scanf("%s", option);

    if (strcmp(option, "a") == 0) {
      while(getchar() != '\n');
      printf("CLIENT: Enter the type1 to send to server ... \n");
      fgets(inStr, sizeof(inStr), stdin);
      inStr[strlen(inStr)-1] = 0;
      
      // Send command string to server
      strcpy(buffer, inStr);
      printf("CLIENT: Sending \"%s\" to server.\n", buffer);
      send(clientSocket, buffer, strlen(buffer), 0);

      int      num_lines = 0;
      Pokemon* read_pokemons; // Initialize a pokemon pointer to save pokemons in one search. 
      char     temp[90];

      while (1) {
        bytesRcv = recv(clientSocket, buffer, 80, 0);
        buffer[bytesRcv] = 0; // put a 0 at the end so we can display the string
        strcpy(temp, buffer);
        
        if (strcmp(buffer, "OK") != 0) {
          num_lines++;
          if (num_lines == 1) {
            read_pokemons = calloc(1, sizeof(Pokemon));
          } else {
            read_pokemons = realloc(read_pokemons,(num_lines+1)*sizeof(Pokemon));
          }
          line_to_pokemon(temp, &(read_pokemons)[num_lines-1], SEPARATOR);
          
          send(clientSocket, "Received", 9, 0);
        } else {
          printf("CLIENT: Got back response \"%s\" from server.\n", buffer);
          break;
        }
      }

      // Allocate the memory for the pokemons array used to save all the completed searches.
      d1->pokemons = realloc(d1->pokemons, (d1->num_pokemons + num_lines)*sizeof(Pokemon));
      if (d1->pokemons == NULL) {
        printf("Error: malloc\n");
        exit(EXIT_FAILURE);
      }

      // Increase the number of queries,
      // and append all the pokemons from one search to polemons array.
      if (sem_wait(&d1->mutex) < 0) {
        printf("Error: on semaphore wait.\n");
        exit(1);
      }
      for (int i=0; i<num_lines; i++) {
        d1->pokemons[d1->num_pokemons + i].pokemon_number = read_pokemons[i].pokemon_number;
        strcpy(d1->pokemons[d1->num_pokemons + i].pokemon_name, read_pokemons[i].pokemon_name);
        strcpy(d1->pokemons[d1->num_pokemons + i].type1, read_pokemons[i].type1);
        strcpy(d1->pokemons[d1->num_pokemons + i].type2, read_pokemons[i].type2);
        d1->pokemons[d1->num_pokemons + i].total = read_pokemons[i].total;
        d1->pokemons[d1->num_pokemons + i].hp = read_pokemons[i].hp;
        d1->pokemons[d1->num_pokemons + i].attack = read_pokemons[i].attack;
        d1->pokemons[d1->num_pokemons + i].defense = read_pokemons[i].defense;
        d1->pokemons[d1->num_pokemons + i].sp_attack = read_pokemons[i].sp_attack;
        d1->pokemons[d1->num_pokemons + i].sp_defense = read_pokemons[i].sp_defense;
        d1->pokemons[d1->num_pokemons + i].speed = read_pokemons[i].speed;
        d1->pokemons[d1->num_pokemons + i].generation = read_pokemons[i].generation;
        d1->pokemons[d1->num_pokemons + i].legendary = read_pokemons[i].legendary;
      }
      d1->count++;
      if (sem_post(&d1->mutex) < 0) {
        printf("Error: on semaphore post.\n");
        exit(1);
      }
      
      // Increase the total number of pokemons saved in the pokemon array.
      d1->num_pokemons += num_lines; 
      free(read_pokemons);
      continue;
    } else if (strcmp(option, "b") == 0) {
      pthread_create(&t1, NULL, saveFunc, (void *) d1);
      pthread_join(t1, NULL);
      continue;
    } else {
      printf("The total number of queries completed: %d\n", d1->count);
      printf("File names: \n");
      char* str = strsep(&(d1->files), SEPARATOR);
      while (str != NULL) {
        if (strcmp(str, "") != 0)
          printf("%s\n",str);
        str = strsep(&(d1->files), SEPARATOR);
      }

      sem_destroy(&d1->mutex);
      
      //Free memory
      free(d1->files);
      free(d1->pokemons);
      free(d1);

      char input[5];
      printf("Do you want to shut down the server (enter 'yes' or 'no'): \n");
      scanf("%s", input);
      if (strcmp(input, "yes") == 0)
        send(clientSocket, "stop", 5, 0);
      else
        send(clientSocket, "done", 5, 0);
      break;
    }
  }
  close(clientSocket);  // Close the socket
  printf("CLIENT: Shutting down.\n");
}