allocate: allocate.o circular_array.o
	gcc -Wall -o allocate allocate.o circular_array.o -lm -g

allocate.o: allocate.c
	gcc -Wall -c allocate.c -g

circular_array.o: circular_array.c
	gcc -Wall -c circular_array.c -g

clean:
	rm -rf allocate circular_array *.o