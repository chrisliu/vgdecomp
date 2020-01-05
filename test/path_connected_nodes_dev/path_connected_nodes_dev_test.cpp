#include <iostream>
#include <string>

#include "../../src/BidirectedGraph.hpp"
#include "../../src/BidirectedGraphBuilder.hpp"
#include "../../src/algorithms/find_path_connected_dev.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    string filename = argv[argc - 1];
    
    BidirectedGraph g = BidirectedGraphBuilder::build_graph(filename);

    auto connected_nodes = find_path_connected_dev(g);

    cout << "Path connected nodes" << endl;
    for (auto& [n_id, nodes] : connected_nodes) {
        cout << "| Node " << n_id << " is path connected to: ";
        for (auto& child_node : nodes) {
            cout << child_node << ", ";
        }
        if (nodes.size()) cout << "\b\b  ";
        cout << endl;
    }

    return 0;
}