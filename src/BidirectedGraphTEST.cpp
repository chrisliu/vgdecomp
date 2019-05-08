#include "BidirectedGraph.hpp"

int main(int argc, char const *argv[])
{
    BidirectedGraph graph1 = BidirectedGraph();
    graph1.add_edge(1, 2, false, false);
    printf("created and added edges to graph1.\n");
    BidirectedGraph graph2 = BidirectedGraph();
    graph2.add_edge(1, 2, false, false);
    graph2.add_edge(2, 3, false, false);
    printf("created and added edges to graph2.\n");
    BidirectedGraph graph3 = BidirectedGraph();
    graph3.add_edge(1, 2, false, false);
    graph3.add_edge(1, 3, false, false);
    graph3.add_edge(2, 4, false, false);
    graph3.add_edge(3, 4, false, false);
    printf("created and added edges to graph3.\n");
    graph1.populate_reachable_nodes();
    printf("populated reachable nodes map in graph1.\n");
    graph2.populate_reachable_nodes();
    printf("populated reachable nodes map in graph2.\n");
    graph3.populate_reachable_nodes();
    printf("populated reachable nodes map in graph3.\n");
    printf("printing reachable nodes in graph1:\n");
    graph1.print_reachable_nodes();

    return 0;
}
