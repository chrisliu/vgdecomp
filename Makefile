# Makefile to compile objects and test file

# Source object
main: build-src

build-src: Edge.o Node.o Graph.o

Edge.o: src/Edge.cpp src/Edge.hpp
	g++ -c src/Edge.cpp -o src/Edge.o

Node.o: src/Node.cpp src/Node.hpp
	g++ -c src/Node.cpp -o src/Node.o

Graph.o: src/Graph.cpp src/Graph.hpp
	g++ -c src/Graph.cpp -o src/Graph.o

Tree.o: src/Tree.cpp src/Tree.hpp
	g++ -c src/Graph.cpp -o src/Graph.o

clean:
	rm src/*.o

# Test file
test: test/main.cpp main
	g++ -c test/main.cpp -o test/main.o
	g++ test/main.o src/*.o -o main

clean-test: clean
	rm main test/main.o
