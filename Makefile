all: table.c
	gcc -Wall -pedantic  -std=c99 table.c -o table
	mv table /usr/bin

clean:
	rm /usr/bin/table
