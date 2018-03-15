Target = mush
CC = gcc
CFLAGS = -pedantic -std=gnu99 -Wall -g
MAIN = mush
OBJS = mush.o parseline.o

all : $(MAIN)

$(MAIN) : $(OBJS)
	$(CC) -o mush $(CFLAGS) mush.c parseline.c parseline.h

mush.o : mush.c
	$(CC) -o $(CFLAGS) -c mush.c

parseline.o : parseline.c
	$(CC) -o $(CFLAGS) -c parseline.c

clean :
	rm *.o $(MAIN) core
