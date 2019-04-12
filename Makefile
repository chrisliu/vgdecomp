# Makefile to compile objects and test file

# Source object
main: build-src

build-src: Edge.o Node.o

Edge.o: src/Edge.cpp src/Edge.hpp
	g++ -c src/Edge.cpp -o src/Edge.o

Node.o: src/Node.cpp src/Node.hpp
	g++ -c src/Node.cpp -o src/Node.o

# Graph and Tree make commands (currently unsupported until Node and Edge have been updated)
# Graph.o: src/Graph.cpp src/Graph.hpp
# 	g++ -c src/Graph.cpp -o src/Graph.o

# Tree.o: src/Tree.cpp src/Tree.hpp
# 	g++ -c src/Graph.cpp -o src/Graph.o

clean:
	rm -f src/*.o

# Test file make commands (currently unsupported until Node and Edge have been updated)
# test: test/main.cpp main
# 	g++ -c test/main.cpp -o test/main.o
# 	g++ test/main.o src/*.o -o main

# clean-test: clean
# 	rm -f main test/main.o
