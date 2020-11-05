CFLAGS_PROD = -O2 -Wall -g
CFLAGS_DEBUG = -Wall -g
.PHONY = all clean %.run

ifdef DEBUG
CFLAGS = $(CFLAGS_DEBUG)
else
CFLAGS = $(CFLAGS_PROD)
endif

all: bin/interp

bin/interp: main.o interp.o interp.h bin
	gcc $(CFLAGS) -o bin/interp main.o interp.o

%.o: %.c
	gcc $(CFLAGS) -c $<

%.test: %.bf bin/interp
	valgrind ./bin/interp $<
	touch $@

%.run: %.bf bin/interp
	./bin/interp $<



clean:
	rm -f bin/interp *.o *.test progs/*.test
bin:
	mkdir -p bin
