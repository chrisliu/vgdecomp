#include <iostream>
#include <string>

#include "../../src/BidirectedGraph.hpp"
#include "../../src/algorithms/is_acyclic.hpp"

using namespace std;

void print_graph_cyclicity(HandleGraph& g, string g_name) {
    cout << "(" << g_name << ") ";
    cout << "is_acyclic: " << (is_acyclic(&g) ? "true" : "false") << endl;
    cout << "(" << g_name << ") ";
    cout << "is_directed_acyclic: " << (is_directed_acyclic(&g) ? "true" : "false") << endl;
}

int main(int argc, char* argv[]) {
    /// g1
    BidirectedGraph g1;
    g1.add_edge(1, 2, false, false);
    g1.add_edge(2, 1, false, false);

    cout << "Graph g1" << endl;
    print_graph_cyclicity(g1, "g1");

    /// g2
    BidirectedGraph g2;
    g2.add_edge(1, 1, false, false);

    cout << "Graph g2" << endl;
    print_graph_cyclicity(g2, "g2");

    /// g3
    BidirectedGraph g3;
    g3.add_edge(1, 1, false, true);

    cout << "Graph g3" << endl;
    print_graph_cyclicity(g3, "g3");

    /// g4
    BidirectedGraph g4;
    g4.add_edge(1, 2, false, false);

    cout << "Graph g4" << endl;
    print_graph_cyclicity(g4, "g4");

    return 0;
}