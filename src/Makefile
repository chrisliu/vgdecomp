# Makefile to compile the graph test

FLAGS = -std=c++11

BidirectedGraphTEST: BidirectedGraph.o BidirectedEdge.o
	g++ $(FLAGS) -c BidirectedGraphTEST.cpp -o BidirectedGraphTEST.o
	g++ $(FLAGS) BidirectedGraphTEST.o BidirectedEdge.o BidirectedGraph.o -o BidirectedGraphTEST

BidirectedGraph.o:
	g++ $(FLAGS) -c BidirectedGraph.cpp -o BidirectedGraph.o

BidirectedEdge.o:
	g++ $(FLAGS) -c BidirectedEdge.cpp -o BidirectedEdge.o

clean:
	rm -f BidirectedGraphTEST.o BidirectedGraph.o BidirectedEdge.o BidirectedGraphTEST
