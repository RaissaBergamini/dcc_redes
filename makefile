# HEADERS = server.h

default: server

server.o: server.c #$(HEADERS)
	gcc -c server.c -o server.o

server: server.o
	gcc server.o -o server

default: client
client.o: client.c #$(HEADERS)
	gcc -c client.c -o client.o

client: client.o
	gcc client.o -o client

clean:
	-rm -f server.o
	-rm -f server
	-rm -f client.o
	-rm -f client