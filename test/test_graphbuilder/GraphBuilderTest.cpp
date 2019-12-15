#include <cstdlib>
#include <string>
#include "../../src/BidirectedGraphBuilder.hpp"

int main(int argc, char* argv[]) {
    BidirectedGraphBuilder builder;
    BidirectedGraph graph = builder.build_graph(argv[1]);
}