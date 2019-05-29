# Makefile to compile objects and test file

FLAGS = -std=c++11

# Source object
main: deps build-src HandleGraphTest

deps: JsonCpp HandleGraph

JsonCpp: src/json/jsoncpp.cpp src/json/json/json-forwards.h src/json/json/json.h
	g++ $(FLAGS) -c src/json/jsoncpp.cpp -o src/jsoncpp.o

HandleGraph: src/handlegraph/handle_graph.hpp src/handlegraph/handle.cpp src/handlegraph/iteratee.hpp src/handlegraph/types.hpp src/handlegraph/util.hpp
	g++ $(FLAGS) -c src/handlegraph/handle.cpp -o src/handle.o

build-src: BidirectedEdge.o BidirectedNode.o BidirectedGraphBuilder.o BidirectedGraph.o

BidirectedEdge.o: src/BidirectedEdge.cpp src/BidirectedEdge.hpp
	g++ $(FLAGS) -c src/BidirectedEdge.cpp -o src/BidirectedEdge.o

BidirectedNode.o: src/BidirectedNode.cpp src/BidirectedNode.hpp
	g++ $(FLAGS) -c src/BidirectedNode.cpp -o src/BidirectedNode.o

BidirectedGraph.o: src/BidirectedGraph.cpp src/BidirectedGraph.hpp
	g++ $(FLAGS) -c src/BidirectedGraph.cpp -o src/BidirectedGraph.o

BidirectedGraphBuilder.o: src/BidirectedGraphBuilder.cpp src/BidirectedGraphBuilder.hpp
	g++ $(FLAGS) -c src/BidirectedGraphBuilder.cpp -o src/BidirectedGraphBuilder.o

GraphBuilderTest: test/GraphBuilderTest.cpp
	g++ $(FLAGS) -c test/GraphBuilderTest.cpp -o test/GraphBuilderTest.o
	g++ test/GraphBuilderTest.o src/*.o -o test/GraphBuilderTest

HandleGraphTest: test/HandleGraphTest.cpp
	g++ $(FLAGS) -g -c test/HandleGraphTest.cpp -o test/HandleGraphTest.o
	g++ test/HandleGraphTest.o src/*.o -o test/HandleGraphTest

clean: clean-source clean-tests

clean-source:
	-rm -f src/*.o

clean-tests: clean-builder clean-handlegraph

clean-builder:
	-rm test/GraphBuilderTest.o test/GraphBuilderTest

clean-handlegraph:
	-rm test/HandleGraphTest.o test/HandleGraphTest

# Test file make commands (currently unsupported until Node and Edge have been updated)
# test: test/main.cpp main
# 	g++ -c test/main.cpp -o test/main.o
# 	g++ test/main.o src/*.o -o main

# clean-test: clean
# 	rm -f main test/main.o
