
// FILE dataProcess.h

#ifndef HEADER_DATAPROCESS
#define HEADER_DATAPROCESS

// DEFINE

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 80
#define MAXLENGTH   32
#define SEPARATOR   ","

// structures

/* Types */
typedef struct PokemonType {
  int pokemon_number;
  char pokemon_name[MAXLENGTH];
  char type1[MAXLENGTH];
  char type2[MAXLENGTH];
  int total;
  int hp;
  int attack;
  int defense;
  int sp_attack;
  int sp_defense;
  int speed;
  int generation;
  char legendary;
} Pokemon;

typedef struct Data {
  volatile int count;
  Pokemon* pokemons;
  char* files;
  int num_pokemons;
  sem_t mutex;
} Data;

/* Function prototypes */
void *saveFunc(void *);
void line_to_pokemon(char* line, Pokemon* new_pokemon, char *separator);
void pokemon_to_line(char* line_to_write, const Pokemon* pokemon_to_write, char *separator);

#endif