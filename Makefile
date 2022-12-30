DEBUG= -g -Wall
EXECS= server client

all:	$(EXECS)

server:	server.c 
	gcc $(DEBUG) -o server server.c

client: client.o dataProcess.o
	gcc $(DEBUG) -o client client.o dataProcess.o -lpthread

client.o: client.c dataProcess.h
	gcc -c client.c

dataProcess.o: dataProcess.c dataProcess.h
	gcc -c dataProcess.c

clean:
	rm -f $(EXECS) *.o
