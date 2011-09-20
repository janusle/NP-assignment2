CC=gcc

all: server-single

server-single: error.o http.o server-single.o
	$(CC) -o server-single error.o http.o server-single.o

server-single.o:
	$(CC) -c server-single.c

error.o: 
	$(CC) -c error.c

http.o:
	$(CC) -c http.c

clean:
	-rm *.o
	-rm server-single
