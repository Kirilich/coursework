CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
OBJ = main.o function.o
EXEC = cw

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c function.h
	$(CC) $(CFLAGS) -c main.c

text_utils.o: function.c function.h
	$(CC) $(CFLAGS) -c function.c

clean:
	rm -f $(OBJ) $(EXEC)
