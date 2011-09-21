CC=gcc
FLAG=-g
all: server-single

server-single: error.o http.o server-single.o
	$(CC) -o server-single error.o http.o server-single.o $(FLAG)

server-single.o:
	$(CC) -c server-single.c $(FLAG)

error.o: 
	$(CC) -c error.c $(FLAG)

http.o:
	$(CC) -c http.c $(FLAG)

clean:
	-rm *.o
	-rm server-single
