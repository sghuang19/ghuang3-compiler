bminor: bminor.c encoder.o scanner.o lex.yy.o parser.o grammar.tab.o
	gcc $^ -o $@

encoder.o: encoder.c
	gcc -c $^ -o $@

lex.yy.c: lex.yy.l
	flex $^

lex.yy.o: lex.yy.c
	gcc -c $^ -o $@

scanner.o: scanner.c token.h
	gcc -c $< -o $@

parser.o: parser.c
	gcc -c $^ -o $@

grammar.tab.c token.h: grammar.y lex.yy.c
	bison --defines=token.h $<
	#bison -v --defines=token.h $<

grammar.tab.o: grammar.tab.c
	gcc -c $^ -o $@

# Tests
test-encoder:
	./runtest.sh encoder

test-scanner:
	./runtest.sh scanner

test-parser:
	./runtest.sh parser

clean:
	rm -f lex.yy.c
	rm -f token.h grammar.tab.c grammar.output
	rm -f *.o
	rm -f ./test/*/*.bminor.out
	rm -f bminor