#not very beautiful but it works
OBJ = obj/IO.o obj/upk.o obj/main.o

#change to your proper compiler
CC = gcc

#modify to fit your needs (these are recommended)
CFLAGS = -Wall -Wextra -g -O2 -I"${CURDIR}/include/"

build: $(OBJ)
	$(CC) -o upk $(OBJ)

obj/IO.o: src/IO.c
	$(CC) $(CFLAGS) -o obj/IO.o -c src/IO.c

obj/upk.o: src/upk.c
	$(CC) $(CFLAGS) -o obj/upk.o -c src/upk.c

obj/main.o: src/main.c
	$(CC) $(CFLAGS) -o obj/main.o -c src/main.c