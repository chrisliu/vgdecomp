# Makefile to compile objects and test file

FLAGS = -std=c++11

# Source object
main: deps build-src GraphBuilderTest

deps: JsonCpp

JsonCpp: src/jsoncpp.cpp src/json/json-forwards.h src/json/json.h
	g++ $(FLAGS) -c src/jsoncpp.cpp -o src/jsoncpp.o

build-src: BidirectedEdge.o BidirectedNode.o BidirectedGraphBuilder.o

BidirectedEdge.o: src/BidirectedEdge.cpp src/BidirectedEdge.hpp
	g++ $(FLAGS) -c src/BidirectedEdge.cpp -o src/BidirectedEdge.o

BidirectedNode.o: src/BidirectedNode.cpp src/BidirectedNode.hpp
	g++ $(FLAGS) -c src/BidirectedNode.cpp -o src/BidirectedNode.o

BidirectedGraphBuilder.o: src/BidirectedGraphBuilder.cpp src/BidirectedGraphBuilder.hpp
	g++ $(FLAGS) -c src/BidirectedGraphBuilder.cpp -o src/BidirectedGraphBuilder.o

GraphBuilderTest: test/GraphBuilderTest.cpp
	g++ $(FLAGS) -c test/GraphBuilderTest.cpp -o test/GraphBuilderTest.o
	g++ test/GraphBuilderTest.o src/*.o -o test/GraphBuilderTest

clean: clean-source clean-tests

clean-source:
	-rm -f src/*.o

clean-tests: clean-builder

clean-builder:
	-rm test/GraphBuilderTest.o test/GraphBuilderTest

# Test file make commands (currently unsupported until Node and Edge have been updated)
# test: test/main.cpp main
# 	g++ -c test/main.cpp -o test/main.o
# 	g++ test/main.o src/*.o -o main

# clean-test: clean
# 	rm -f main test/main.o
