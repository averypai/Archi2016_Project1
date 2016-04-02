CC=g++ -std=c++11
single_cycle: main.o function.o
	g++ -o single_cycle main.o function.o

main.o: main.c function.c function.h
	g++ -c main.c
function.o: function.c function.h
	g++ -c function.c
clean:
	rm -f single_cycle main.o function.o

