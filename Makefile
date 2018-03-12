Target = mush
CC = gcc
CFLAGS = -pedantic -std=gnu99 -Wall -g
MAIN = mush
OBJS = mush.o

all : $(MAIN)

$(MAIN) : $(OBJS)
	$(CC) -o $(MAIN) $(CFLAGS) mush.c

mush.o : mush.c
	$(CC) -o $(CFLAGS) -c mush.c

clean :
	rm *.o $(MAIN) core
