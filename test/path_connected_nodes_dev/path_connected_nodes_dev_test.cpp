#include <iostream>
#include <vector>
#include <unordered_set>
#include <string>

#include "../../src/BidirectedGraph.hpp"
#include "../../src/BidirectedGraphBuilder.hpp"
#include "../../src/algorithms/strongly_connected_components.hpp"
#include "../../src/algorithms/sccgraph.hpp"
#include "../../src/algorithms/topological_sort.hpp"

using namespace std;

void inline print_gname(string g_name) {
    cout << "(" << g_name << ") ";
}

void print_sccs(vector<unordered_set<nid_t>> scc, string g_name) {
    cout << "Graph " << g_name << endl;
    for (int i = 0; i < scc.size(); i++) {
        print_gname(g_name);
        cout << "SCC " << i << " nodes: ";
        for (auto& node : scc[i]) {
            cout << node << ", ";
        }
        cout << "\b\b  " << endl;
    }
}

int main(int argc, char* argv[]) {
    string filename = "00_trivial_scc_graph.json";
    // string filename = "01_serial_and_parallel_nodes.json";
    BidirectedGraph g = BidirectedGraphBuilder::build_graph(filename);

    cout << "(debug) BidirectedGraph" << endl;
    g.for_each_handle([&](const handle_t& handle) {
        cout << "(debug) | Node " << g.get_id(handle) << endl;
        
        bool printed = false;
        cout << "(debug) | | Left nodes: ";
        g.follow_edges(handle, true, [&](const handle_t& child_handle) {
            cout << g.get_id(child_handle) << ", ";
            printed = true;
        });
        if (printed) cout << "\b\b  ";
        cout << endl;
        printed = false;
        cout << "(debug) | | Right nodes: ";
        g.follow_edges(handle, false, [&](const handle_t& child_handle) {
            cout << g.get_id(child_handle) << ", ";
            printed = true;
        });
        if (printed) cout << "\b\b  ";
        cout << endl;
    });
    
    auto sccs = strongly_connected_components(&g);
    print_sccs(sccs, filename);

    SCCGraph scc_g(g, sccs);

    print_gname(filename);
    cout << "SCC graph nodes: " << endl;
    scc_g.for_each_handle([&](const handle_t& handle) {
        nid_t node_id = scc_g.get_id(handle);

        print_gname(filename);
        cout << "| SCC " << scc_g.get_id(handle) << endl;
        
        bool printed = false;
        print_gname(filename);
        cout << "| | Left side: ";
        scc_g.follow_edges(handle, true, [&](const handle_t& child_handle) {
            cout << g.get_id(child_handle) << (g.get_is_reverse(child_handle) ? "l" : "r") << ", ";
            printed = true;
        });
        if (printed) cout << "\b\b  ";
        cout << endl;
        printed = false;
        print_gname(filename);
        cout << "| | Right side: ";
        scc_g.follow_edges(handle, false, [&](const handle_t& child_handle) {
            cout <<  g.get_id(child_handle) << (g.get_is_reverse(child_handle) ? "r" : "l") << ", ";
            printed = true;
        });
        if (printed) cout << "\b\b  ";
        cout << endl;
    });

    auto topo_order = topological_order(&scc_g);

    bool printed = false;
    print_gname(filename);
    cout << "Topo order: ";
    for (auto& handle : topo_order) {
        cout << scc_g.get_id(handle) << ", ";
        printed = true;
    }
    if (printed) cout << "\b\b  ";
    cout << endl;

    return 0;
}