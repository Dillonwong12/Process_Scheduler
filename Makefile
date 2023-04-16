allocate: allocate.o circular_array.o memory.o
	gcc -Wall -o allocate allocate.o circular_array.o memory.o -lm -g

allocate.o: allocate.c circular_array.h memory.h
	gcc -Wall -c allocate.c -g

circular_array.o: circular_array.h circular_array.c
	gcc -Wall -c circular_array.c -g

memory.o: memory.h memory.c
	gcc -Wall -c memory.c -g

clean:
	rm -rf allocate circular_array memory process *.o