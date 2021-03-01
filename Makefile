all: table.c
	gcc table.c -o table
	mv table /usr/bin

clean:
	rm /usr/bin/table
