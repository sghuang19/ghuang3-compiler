bminor: bminor.c encoder.o scanner.o lex.yy.o
	gcc $^ -o $@

encoder.o: encoder.c
	gcc -c $^ -o $@

lex.yy.c: lex.yy.l
	flex $^

lex.yy.o: lex.yy.c
	gcc -c $^ -o $@

scanner.o: scanner.c
	gcc -c $^ -o $@

# Tests
test-encoder:
	./runtest.sh encoder

test-scanner:
	./runtest.sh scanner

clean:
	rm -f lex.yy.c
	rm -f *.o
	rm -f ./test/*/*.bminor.out
	rm -f bminor