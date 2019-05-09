#include "BidirectedGraph.hpp"

int main(int argc, char const *argv[])
{
    BidirectedGraph graph1;
    graph1.add_edge((uint64_t)1, (uint64_t)2, false, false);
    printf("created and added edges to graph1.\n");
    BidirectedGraph graph2;
    graph2.add_edge((uint64_t)1, (uint64_t)2, false, false);
    graph2.add_edge((uint64_t)2, (uint64_t)3, false, false);
    printf("created and added edges to graph2.\n");
    BidirectedGraph graph3;
    graph3.add_edge((uint64_t)1, (uint64_t)2, false, false);
    graph3.add_edge((uint64_t)1, (uint64_t)3, false, false);
    graph3.add_edge((uint64_t)2, (uint64_t)4, false, false);
    graph3.add_edge((uint64_t)3, (uint64_t)4, false, false);
    printf("created and added edges to graph3.\n");
    graph1.populate_reachable_nodes();
    printf("populated reachable nodes map in graph1.\n");
    graph2.populate_reachable_nodes();
    printf("populated reachable nodes map in graph2.\n");
    graph3.populate_reachable_nodes();
    printf("populated reachable nodes map in graph3.\n");
    graph1.print_reachable_nodes();
    graph2.print_reachable_nodes();
    graph3.print_reachable_nodes();


    printf("finished tests.");

    return 0;
}
