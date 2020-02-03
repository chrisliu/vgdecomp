#include <iostream>
#include <string>
#include <fstream>

#include "../../src/BidirectedGraph.hpp"
#include "../../src/algorithms/find_balanced_bundles.hpp"
#include "../../src/algorithms/reduce.hpp"

using namespace std;

/// Takes an input graph and outputs if a reduction action is possible
int main(int argc, char* argv[]) {
    /// Load file
    string filename = argv[argc - 1];
    ifstream json_file(filename, ifstream::binary);
    BidirectedGraph g;
    g.deserialize(json_file);    

    /// Construct bundle_map 
    bundle_map_t bundle_map;
    auto bundles = find_balanced_bundles(g);
    for (auto& bundle : bundles) mark_bundle(g, bundle, bundle_map);

    /// Traverse each node and output if a traversal is possible
    g.for_each_handle([&](const handle_t& handle) {
        nid_t node_id = g.get_id(handle);
        handle_t return_handle;
        cout << "Node " <<  node_id << endl;
        cout << "> RA1: " << (is_reduction1(g, node_id, return_handle) ? "Y" : "N") << endl;
        cout << "> RA2: " << (is_reduction2(g, node_id, bundle_map, return_handle) ? "Y" : "N") << endl;
        cout << "> RA3: " << (is_reduction3(g, node_id, bundle_map, return_handle) ? "Y" : "N") << endl;
    });

    return EXIT_SUCCESS;
}