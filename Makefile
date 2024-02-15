CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I./raylib/src -DPLATFORM_DESKTOP
LDFLAGS =  -lpthread -lraylib -lm 

SRC = main.c

all: mine

mine: $(SRC)
	$(CC) $(SRC) -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f mine
