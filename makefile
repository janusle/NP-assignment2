CC=gcc
FLAG=-g #-lsocket -lnsl
all: server-single server-forked

server-forked: error.o http.o server-forked.o confprocess.o 
	$(CC) -o server-forked error.o http.o server-forked.o confprocess.o $(FLAG)

server-single: error.o http.o server-single.o confprocess.o
	$(CC) -o server-single error.o http.o server-single.o confprocess.o $(FLAG)

server-forked.o:
	$(CC) -c server-forked.c $(FLAG)

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
