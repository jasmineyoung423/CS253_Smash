CFLAGS = -Wall -std=c99

OBJS = history.o commands.o


%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<

default: all

all: rules.d smash

rules.d: $(wildcard *.c) $(wildcard *.h)
	gcc -MM $(wildcard *.c) > rules.d

-include rules.d

smashLib.a: $(OBJS)
	ar r  $@ $?

smash: smash.o smashLib.a
	gcc -o $@ $^

smash.o: smash.c smash.h commands.c history.c history.h
	gcc $(CFLAGS) -c smash.c -o smash.o

clean:
	rm *.o smashLib.a smash rules.d

