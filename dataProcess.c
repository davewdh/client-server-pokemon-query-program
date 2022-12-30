// FILE is dataProcess.c

// INCLUDE
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

/*
* Function: saveFunc
* Description:
*   writes the contents of the pokemon array to a file that provided by the user
* Parameters:
 *   Data struct: volatile int count; Pokemon* pokemons; char *files;
                  int num_pokemons; sem_t mutex;
 * Returns:
 *   None
*/
void *saveFunc(void *arg) {
  Data *inc = (Data *)arg;

  // Ask the user for the file name to save the pokemons.
  FILE* data_csv_file = NULL;
  char file_name[MAXLENGTH];
  printf("Enter the name of file to save the results: \n");
  while (1) {
    if (strcmp(file_name, "q") == 0)
      exit(EXIT_FAILURE);
    scanf("%s", file_name);
    data_csv_file = fopen(file_name, "w");
    if (!data_csv_file) {
      printf("Unable to create the new file. Please enter the name of the file again or press 'q' to qiut.\n");
      continue;
    } else {
      break;
    }
  }

  // Concatenate the filename in a string for later display.
  if (strstr(inc->files, file_name) == NULL) {
    inc->files = strcat(inc->files, file_name);
    inc->files = strcat(inc->files, SEPARATOR);
  }

  for(int i = 0; i < inc->num_pokemons; i++){
    if (!data_csv_file) {
      printf("***************************\n");
      printf("Printing pokemon: %d\n", i);
      printf("Printing pokemon: %d\n", inc->pokemons[i].pokemon_number);
      printf("Name: %s\n", inc->pokemons[i].pokemon_name);
    } else {
      char line[90] = ""; /* Initialize the string to use strcat */
      pokemon_to_line(line, &inc->pokemons[i], SEPARATOR);
      fprintf(data_csv_file, "%s\n", line);
    }
  }

  if (data_csv_file != NULL) {
    fclose(data_csv_file);
    printf("Pokemon Data written to: %s\n", file_name);
  } else {
    printf("***************************\n");
    printf("Exiting!\n");
  }
  return (0);
}

/*
* Function: line_to_pokemon
* Description: converts a csv line to a PokemonType Data structure
* Parameters:
*    line: the line to be converted to a PokemonType
*    new_pokemon: the new PokemonType to be populated with Data
*    separator: the character to use as the sparator, ',' by default
*/
void line_to_pokemon(char* line, Pokemon* new_pokemon, char *separator) {

  char *pokemon_number = strsep(&line, separator);
  char *pokemon_name = strsep(&line, separator);
  char *type1 = strsep(&line, separator);
  char *type2 = strsep(&line, separator);
  char *total = strsep(&line, separator);
  char *hp = strsep(&line, separator);
  char *attack = strsep(&line, separator);
  char *defense = strsep(&line, separator);
  char *sp_attack = strsep(&line, separator);
  char *sp_defense = strsep(&line, separator);
  char *speed = strsep(&line, separator);
  char *generation = strsep(&line, separator);
  char *legendary = strsep(&line, separator);

  strcpy(new_pokemon->pokemon_name, pokemon_name);
  strcpy(new_pokemon->type1, type1);
  strcpy(new_pokemon->type2, type2);
  if (strcmp(legendary, "True") == 0)
    new_pokemon->legendary = 1;
  else 
    new_pokemon->legendary = 0;
  
  // Assumes token is an empty string or a number.
  new_pokemon->pokemon_number = strcmp(pokemon_number, "") ? atoi(pokemon_number) : -1;
  new_pokemon->total = strcmp(total, "") ? atoi(total) : -1;
  new_pokemon->hp = strcmp(hp, "") ? atoi(hp) : -1;
  new_pokemon->attack = strcmp(attack, "") ? atoi(attack) : -1;
  new_pokemon->defense = strcmp(defense, "") ? atoi(defense) : -1;
  new_pokemon->sp_attack = strcmp(sp_attack, "") ? atoi(sp_attack) : -1;
  new_pokemon->sp_defense = strcmp(sp_defense, "") ? atoi(sp_defense) : -1;
  new_pokemon->speed = strcmp(speed, "") ? atoi(speed) : -1;
  new_pokemon->generation = strcmp(generation, "") ? atoi(generation) : -1;
}

/*
* Function: pokemon_to_line
* Description:
*   converts a PokemonType Data structure to a csv string to be written to a file
* Parameters:
*   line_to_write: string to hold the PokemonType Data
*   pokemon_to_write: pointer to the PokemonType structure that will populate the
*                     line_to_write string
*   separator: the separator character to use, ',' by default
*/
void pokemon_to_line(char* line_to_write, const Pokemon* pokemon_to_write, char *separator) {
  char pokemon_num[MAXLENGTH];
  sprintf(pokemon_num, "%d", pokemon_to_write->pokemon_number);
  line_to_write = strcat(line_to_write, pokemon_num);
  line_to_write = strcat(line_to_write, separator);
  line_to_write = strcat(line_to_write, pokemon_to_write->pokemon_name);
  line_to_write = strcat(line_to_write, separator);
  line_to_write = strcat(line_to_write, pokemon_to_write->type1);
  line_to_write = strcat(line_to_write, separator);
  line_to_write = strcat(line_to_write, pokemon_to_write->type2);
  line_to_write = strcat(line_to_write, separator);
  char pokemon_total[MAXLENGTH];
  sprintf(pokemon_total, "%d", pokemon_to_write->total);
  line_to_write = strcat(line_to_write, pokemon_total);
  line_to_write = strcat(line_to_write, separator);
  char pokemon_hp[MAXLENGTH];
  sprintf(pokemon_hp, "%d", pokemon_to_write->hp);
  line_to_write = strcat(line_to_write, pokemon_hp);
  line_to_write = strcat(line_to_write, separator);
  char pokemon_attack[MAXLENGTH];
  sprintf(pokemon_attack, "%d", pokemon_to_write->attack);
  line_to_write = strcat(line_to_write, pokemon_attack);
  line_to_write = strcat(line_to_write, separator);
  char pokemon_defense[MAXLENGTH];
  sprintf(pokemon_defense, "%d", pokemon_to_write->defense);
  line_to_write = strcat(line_to_write, pokemon_defense);
  line_to_write = strcat(line_to_write, separator);
  char pokemon_sp_attack[MAXLENGTH];
  sprintf(pokemon_sp_attack, "%d", pokemon_to_write->sp_attack);
  line_to_write = strcat(line_to_write, pokemon_sp_attack);
  line_to_write = strcat(line_to_write, separator);
  char pokemon_sp_defense[MAXLENGTH];
  sprintf(pokemon_sp_defense, "%d", pokemon_to_write->sp_defense);
  line_to_write = strcat(line_to_write, pokemon_sp_defense);
  line_to_write = strcat(line_to_write, separator);
  char pokemon_speed[MAXLENGTH];
  sprintf(pokemon_speed, "%d", pokemon_to_write->speed);
  line_to_write = strcat(line_to_write, pokemon_speed);
  line_to_write = strcat(line_to_write, separator);
  char pokemon_generation[MAXLENGTH];
  sprintf(pokemon_generation, "%d", pokemon_to_write->generation);
  line_to_write = strcat(line_to_write, pokemon_generation);
  line_to_write = strcat(line_to_write, separator);
  if (pokemon_to_write->legendary)
    line_to_write = strcat(line_to_write, "True");
  else 
    line_to_write = strcat(line_to_write, "False");
}


