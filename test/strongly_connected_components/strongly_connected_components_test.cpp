#include <iostream>
#include <string>

#include "../../src/BidirectedGraph.hpp"
#include "../../src/algorithms/strongly_connected_components.hpp"

using namespace std;

void inline print_gname(string g_name) {
    cout << "(" << g_name << ") ";
}

void print_sccs(vector<unordered_set<nid_t>> scc, string g_name) {
    cout << "Graph " << g_name << endl;
    for (int i = 0; i < scc.size(); i++) {
        print_gname(g_name);
        cout << "SCC " << (i + 1) << " nodes: ";
        for (auto& node : scc[i]) {
            cout << node << ", ";
        }
        cout << "\b\b  " << endl;
    }
}

int main(int argc, char* argv[]) {
    BidirectedGraph g1;
    g1.add_edge(1, 2, false, false);
    g1.add_edge(2, 3, false, false);
    g1.add_edge(3, 1, false, false);
    g1.add_edge(3, 4, false, false);

    auto sccs1 = strongly_connected_components(&g1);
    print_sccs(sccs1, "g1");


    BidirectedGraph g2;
    g2.add_edge(1, 2, false, false);
    g2.add_edge(2, 3, false, false);
    g2.add_edge(3, 1, false, false);
    g2.add_edge(3, 2, false, false);

    auto sccs2 = strongly_connected_components(&g2);
    print_sccs(sccs2, "g2");

    BidirectedGraph g3;
    g3.add_edge(1, 2, false, false);
    g3.add_edge(2, 3, false, false);

    auto sccs3 = strongly_connected_components(&g3);
    print_sccs(sccs3, "g3");

    BidirectedGraph g4;
    g4.add_edge(1, 1, false, false);
    g4.add_edge(1, 2, false, false);

    auto sccs4 = strongly_connected_components(&g4);
    print_sccs(sccs4, "g4");

    BidirectedGraph g5;
    g5.add_edge(1, 2, false, false);
    g5.add_edge(2, 3, false, false);
    g5.add_edge(1, 4, false, false);
    g5.add_edge(4, 1, false, false);
    g5.add_edge(4, 2, false, false);

    auto sccs5 = strongly_connected_components(&g5);
    print_sccs(sccs5, "g5");

    BidirectedGraph g6;
    g6.add_edge(1, 2, false, false);
    g6.add_edge(2, 3, false, false);
    g6.add_edge(1, 4, false, false);
    g6.add_edge(4, 1, false, false);
    g6.add_edge(2, 4, false, false);

    auto sccs6 = strongly_connected_components(&g6);
    print_sccs(sccs6, "g6");

    BidirectedGraph g7;
    g7.add_edge(1, 2, false, false);
    g7.add_edge(2, 3, false, false);
    g7.add_edge(3, 1, false, true);
    g7.add_edge(1, 3, true, false);

    auto sccs7 = strongly_connected_components(&g7);
    print_sccs(sccs7, "g7");


    BidirectedGraph g8;
    g8.add_edge(1, 2, false, false);
    g8.add_edge(2, 1, false, false);
    g8.add_edge(1, 3, false, false);
    g8.add_edge(2, 3, true, false);

    auto sccs8 = strongly_connected_components(&g8);
    print_sccs(sccs8, "g8");

    return 0;
}