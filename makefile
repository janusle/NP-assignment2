CC=gcc
FLAG=-g #-lsocket -lnsl
all: server-single

server-single: error.o http.o server-single.o confprocess.o
	$(CC) -o server-single error.o http.o server-single.o confprocess.o $(FLAG)

server-single.o:
	$(CC) -c server-single.c $(FLAG)

confprocess.o:
	$(CC) -c confprocess.c $(FLAG)

error.o: 
	$(CC) -c error.c $(FLAG)

http.o:
	$(CC) -c http.c $(FLAG)

clean:
	-rm *.o
	-rm server-single
