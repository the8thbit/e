CC=gcc
CFLAGS=-Wall -Wextra -pedantic
OBJ=e.o codes.o
RM=rm -f

all: e

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

e: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:
	$(RM) $(OBJ)
