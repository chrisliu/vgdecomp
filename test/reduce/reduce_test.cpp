#include <iostream>
#include <string>
#include <fstream>

#include "../../src/BidirectedGraph.hpp"
#include "../../src/algorithms/reduce.hpp"

using namespace std;

/// Takes an input graph and outputs if a reduction action is possible
int main(int argc, char* argv[]) {
    /// Load file
    string filename = argv[argc - 1];
    ifstream json_file(filename, ifstream::binary);
    BidirectedGraph g;
    g.deserialize(json_file);    

    // test(g);
    reduce_graph(g);

    cout << "-------- Final Output ---------" << endl;
    cout << "Graph size: " << g.get_node_count() << endl;
    g.for_each_handle([&](const handle_t& handle) {
        cout << "Node " << g.get_id(handle) << endl;
        cout << "Left neighbors: ";
        g.follow_edges(handle, true, [&](const handle_t& l_nei) { 
            cout << g.get_id(l_nei) << (g.get_is_reverse(l_nei) ? "r" : "") << ", ";
        });
        if (g.get_degree(handle, true)) cout << "\b\b  ";
        cout << endl;

        cout << "Right neighbors: ";
        g.follow_edges(handle, false, [&](const handle_t& r_nei) { 
            cout << g.get_id(r_nei) << (g.get_is_reverse(r_nei) ? "r" : "") << ", ";
        });
        if (g.get_degree(handle, false)) cout << "\b\b  ";
        cout << endl;
    });

    return EXIT_SUCCESS;
}