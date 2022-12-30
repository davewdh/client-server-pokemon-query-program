There are three c files and one header file in this program, and we use client/server model (TCP). In this 
model, one process acts as a server that receives requests from clients and then performs tasks 
accordingly. The server.c will communicate with clients and return the requested information to the 
client, and the client.c will use a while loop to ask user for the name of the file containing the Pokemon 
descriptions. Once opening the file successfully, it always prompts a menu for the user to choose among 
search for files, save the search, and exit the program. The main thread will send the type1 that user 
provided to server and temporary save the information from server if user choose option a. If user 
choose b, one thread will be created to save the search pokemons (only completed search will be 
saved). If user choose c, the total number of queries completed and file names used to save search will 
be displayed, then the program will be terminated. Line_to_pokemon function and pokemon_to_line 
function are helper functions to either convert a line from the file to a pokemon struct or convert a 
pokemon struct to a string.  
 
Starting with the server, we need to create a stream socket. This can be done with the socket() function 
which is defined in the <sys/socket.h> header, and the <netinet/in.h> header file contains definitions for 
the internet protocol family. Also, the <arpa/inet.h> header makes available the type in_port_t and the 
type in_addr_t as defined in the description of <netinet/in.h>. In order to create threads, we use the 
pthread_create() function which is defined in the <pthread.h> header file. A semaphore is defined as a 
sem_t type and we need to include the <semaphore.h> header in our code in order to use it. 
 
We set each pokemon as a struct and save it in a pokemon array in a search, then append this array to a 
large array for later use. In order to protect these shared data from corruption, we use semaphores to 
accomplish this. The semaphore acts as a locking mechanism to prevent other threads from accessing or 
modifying a resource at the same time. While the resource is locked, other threads are waiting.  


instructions for compiling and running the source files:

	all:	server client

	server:	server.c 
		gcc -o server server.c

	client: client.o dataProcess.o
		gcc -o client client.o dataProcess.o -lpthread

	client.o: client.c dataProcess.h
		gcc -c client.c

	dataProcess.o: dataProcess.c dataProcess.h
		gcc -c dataProcess.c

	clean:
		rm -f server client *.o