CC = gcc
.DEFAULT_GOAL = bminor

encoder.o: encoder.c
	$(CC) -c $^ -o $@

bminor: bminor.c encoder.o
	$(CC) $^ -o $@

clean:
	rm -f *.o
	rm -f bminor