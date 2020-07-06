#pcbid-generator makefile

CC=gcc
CFLAGS=-g -Wall -lm -lcrypto
TARGET=pcbid-generator
PREFIX=/usr/local/bin

SRC=main.c utils.c
OBJ=$(SRC:.c=.o)

all:	$(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(CFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)

install:
	install -c -m 755 $(TARGET) $(PREFIX)/$(TARGET)
