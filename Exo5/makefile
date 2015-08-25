CFLAGS=-Wall -g -std=c99 -Wextra
LDFLAGS=

all: main clean

main: serveur client	

serveur: serveur.o fonction_reseau.o
	gcc -o receiver serveur.o fonction_reseau.o $(LDFLAGS)

client: client.o fonction_reseau.o
	gcc -o sender client.o fonction_reseau.o $(LDFLAGS)

serveur.o: serveur.c fonction_reseau.h type_buffer.h
	gcc -o serveur.o -c serveur.c $(CFLAGS)

client.o: client.c fonction_reseau.h type_buffer.h
	gcc -o client.o -c client.c $(CFLAGS)

fonction_reseau.o:
	gcc -o fonction_reseau.o -c fonction_reseau.c $(CFLAGS)	

.PHONY: clean mrproper

clean:
	rm -rf *.o

mrproper: clean
	rm -rf sender
	rm -rf receiver
			