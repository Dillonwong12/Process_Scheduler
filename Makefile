allocate: allocate.c
	gcc -Wall -o allocate allocate.c

clean:
	rm -rf allocate