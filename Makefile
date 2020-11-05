CFLAGS_PROD = -O2 -Wall -g
CFLAGS_DEBUG = -Wall -g -D DEBUG
VALGRIND_FLAGS = --leak-check=full --error-exitcode=1
.PHONY = all clean test %.run

TEST_OBJS = $(addprefix test/, main.o unit.o) lib/file.o interp.o

ifdef DEBUG
CFLAGS = $(CFLAGS_DEBUG)
else
CFLAGS = $(CFLAGS_PROD)
endif

all: bin/interp bin/test

bin/interp: main.o interp.o interp.h bin
	gcc $(CFLAGS) -o bin/interp main.o interp.o

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

# ==============================================================================

test: bin/test
	valgrind $(VALGRIND_FLAGS) $<

bin/test: $(TEST_OBJS)
	gcc $(CFLAGS_DEBUG) -o $@ $(TEST_OBJS)

test/main.o: test/main.c lib/file.h test/unit.h test/types.h interp.h

%.test: %.bf bin/interp
	valgrind $(VALGRIND_FLAGS) ./bin/interp $<
	touch $@

%.run: %.bf bin/interp
	./bin/interp $<

# ==============================================================================

clean:
	rm -f bin/interp *.o *.test progs/*.test
bin:
	mkdir -p bin
