CFLAGS = -Wall -Wextra -g
MODS = encoder scanner parser printer resolver typechecker codegen
SRCS = $(shell find . -maxdepth 1 -type f -name "*.c")
OBJS = $(SRCS:.c=.o)

bminor: grammar.tab.o lex.yy.o $(OBJS)
	gcc $(CFLAGS) $^ -o $@

lex.yy.o: lex.yy.c
	gcc -c $< -o $@  # suppress warnings

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

lex.yy.c: lex.yy.l
	flex $<

grammar.tab.c token.h: grammar.y
	bison --defines=token.h $<

# Tests
TESTS = $(addprefix test-, $(MODS))
test-%: bminor
	./runtest.sh $(@:test-%=%)

# Cleanup
clean-test:
	rm -f ./test/*/*.bminor.out

clean: clean-test
	rm -f lex.yy.c
	rm -f token.h grammar.tab.c grammar.output
	rm -f *.o
	rm -f bminor
