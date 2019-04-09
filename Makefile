# Makefile

main: src

src: build-src

build-src: Edge.o Node.o

Edge.o: src/Edge.cpp src/Edge.hpp
	g++ -c src/Edge.cpp -o src/Edge.o

Node.o: src/Node.cpp src/Node.hpp
	g++ -c src/Node.cpp -o src/Node.o

clean:
	rm src/*.o

test: test/main.cpp main
	g++ -c test/main.cpp -o test/main.o
	g++ test/main.o src/*.o -o main

clean-test: clean
	rm main