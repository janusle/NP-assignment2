CC=gcc
FLAG=-g -lsocket -lnsl -Wall -w -pedantic#-pthread #-Wall
all: server-single server-forked server-threaded server-select gethttp-slow

gethttp-slow: libhttp.o error.o gethttp.o
	$(CC) -o gethttp-slow gethttp.o error.o libhttp.o $(FLAG)

server-select: error.o http.o server-select.o confprocess.o
	$(CC) -o server-select error.o http.o server-select.o confprocess.o $(FLAG) 

server-threaded: error.o http.o server-threaded.o confprocess.o
	$(CC) -o server-threaded error.o http.o server-threaded.o confprocess.o $(FLAG) -pthread

server-forked: error.o http.o server-forked.o confprocess.o 
	$(CC) -o server-forked error.o http.o server-forked.o confprocess.o $(FLAG)

server-single: error.o http.o server-single.o confprocess.o
	$(CC) -o server-single error.o http.o server-single.o confprocess.o $(FLAG)

gethttp.o:
	$(CC) -c gethttp.c $(FLAG)

libhttp.o:
	$(CC) -c libhttp.c $(FLAG)

server-select.o:
	$(CC) -c server-select.c $(FLAG)

server-threaded.o:
	$(CC) -c server-threaded.c $(FLAG) -pthread

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
	-rm server-forked
	-rm server-threaded
	-rm server-select
	-rm gethttp-slow
	-rm gethttp-tx
	-rm gethttp-rx
	-rm *.mp3
	-rm *.dat
	-rm *.jpg
	-rm *.png
	-rm *.log
	-rm *.txt
	-rm core
