bminor: bminor.c \
		encoder.o \
		scanner.o lex.yy.o \
		parser.o grammar.tab.o \
		printer.o decl.o stmt.o expr.o type.o param_list.o \
		resolver.o symbol.o scope.o hash_table.o
	gcc $^ -o $@

encoder.o: encoder.c encoder.h
	gcc -c $< -o $@

# Scanner
lex.yy.c: lex.yy.l
	flex $<

lex.yy.o: lex.yy.c
	gcc -c $< -o $@

scanner.o: scanner.c token.h
	gcc -c $< -o $@

# Parser
parser.o: parser.c printer.h
	gcc -c $< -o $@

grammar.tab.c token.h: grammar.y lex.yy.c
	bison --defines=token.h $<

grammar.tab.o: grammar.tab.c
	gcc -c $^ -o $@

# Printer
printer.o: printer.c printer.h
	gcc -c $< -o $@

decl.o: decl.c decl.h
	gcc -c $< -o $@

stmt.o: stmt.c stmt.h
	gcc -c $< -o $@

expr.o: expr.c expr.h
	gcc -c $< -o $@

type.o: type.c type.h
	gcc -c $< -o $@

param_list.o: param_list.c param_list.h
	gcc -c $< -o $@

# Resolver
resolver.o: resolver.c resolver.h
	gcc -c $< -o $@

symbol.o: symbol.c symbol.h
	gcc -c $< -o $@

scope.o: scope.c scope.h
	gcc -c $< -o $@

hash_table.o: hash_table.c hash_table.h
	gcc -c $< -o $@

# Tests
test-encoder:
	./runtest.sh encoder

test-scanner:
	./runtest.sh scanner

test-parser:
	./runtest.sh parser

test-printer:
	./runtest.sh printer

test-resolver:
	./runtest.sh resolver

# Cleanup
clean-test:
	rm -f ./test/*/*.bminor.out

clean:
	rm -f ./test/*/*.bminor.out
	rm -f lex.yy.c
	rm -f token.h grammar.tab.c grammar.output
	rm -f *.o
	rm -f bminor
