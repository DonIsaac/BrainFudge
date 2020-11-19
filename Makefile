CFLAGS_PROD =    -O2 -Wall -D NDEBUG
CFLAGS_DEBUG =   -Wall -g -D DEBUG
VALGRIND_FLAGS = --leak-check=full --error-exitcode=1
.PHONY =         all clean test %.run

LIB_OBJS =  $(addprefix lib/, err.o file.o)
TEST_OBJS = $(addprefix test/, main.o unit.o) $(LIB_OBJS) src/interp.o
EXE_OBJS =  $(LIB_OBJS) src/main.o

ifdef DEBUG
CFLAGS = $(CFLAGS_DEBUG)
else
CFLAGS = $(CFLAGS_PROD)
endif

all: bin/interp bin/test

bin/%: src/%.o $(EXE_OBJS) bin
	gcc $(CFLAGS) -o $@ $< $(EXE_OBJS)

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

# ==============================================================================

test: bin/test
	valgrind $(VALGRIND_FLAGS) $<

bin/test: $(TEST_OBJS)
	gcc $(CFLAGS) -o $@ $(TEST_OBJS)

test/main.o: test/main.c lib/file.h test/unit.h test/types.h src/interp.h

%.test: %.bf bin/interp
	valgrind $(VALGRIND_FLAGS) ./bin/interp $<
	touch $@

%.run: %.bf bin/interp
	./bin/interp $<

# ==============================================================================

clean:
	rm -f bin/* $$(find . -name '*.o' -or -name '*.test')

bin:
	mkdir -p bin
