#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "../../src/BidirectedGraph.hpp"
#include "../../src/algorithms/find_balanced_bundles.hpp"
#include "../../src/algorithms/bundle.hpp"

#define DEBUG_BIDIRECTED_GRAPH

using namespace std;

std::string node_to_str(const handle_t& handle, const HandleGraph& g) {
    stringstream ss;
    ss << "Node " << g.get_id(handle);
    if (g.get_is_reverse(handle)) ss << " reversed";
    return ss.str();
}

void print_bundle(BidirectedGraph& g, Bundle& bundle) {
    cout << "Left" << endl;
    for (const auto& l_handle : bundle.get_left()) {
        cout << node_to_str(l_handle, g) << endl;
    }
    cout << "Right" << endl;
    for (const auto& r_handle: bundle.get_right()) {
        cout << node_to_str(r_handle, g) << endl;
    }; 
    cout << "Is a trivial bundle:  " << ((bundle.is_trivial()) ? "true" : "false") << endl;
    cout << "Has reversed node(s): " << ((bundle.has_reversed_node()) ? "true" : "false") << endl;
}

void print_edges(BidirectedGraph& g) {
    g.for_each_handle([&](const handle_t& handle) {
        bool printed = false;
        cout << "Node " << g.get_id(handle) << " left nodes: ";
        g.follow_edges(handle, true, [&](const handle_t& child_handle) {
            cout << g.get_id(child_handle);
            if (g.get_is_reverse(g.flip(handle)) != g.get_is_reverse(child_handle)) {
                cout << "r";
            }
            cout << ", ";
            printed = true;
        });
        if (printed) cout << "\b\b  ";
        cout << endl;

        printed = false;
        cout << "Node " << g.get_id(handle) << " right nodes: ";
        g.follow_edges(handle, false, [&](const handle_t& child_handle) {
            cout << g.get_id(child_handle);
            if (g.get_is_reverse(handle) != g.get_is_reverse(child_handle)) {
                cout << "r";
            }
            cout << ", ";
            printed = true;
        });
        if (printed) cout << "\b\b  ";
        cout << endl;
    });
}

/// Usage:
/// Call BundleTest binary with the test graph json files as the arguments
/// Ex: ./BundleTest test/bundle_test_graph/00_trivial.json
int main(int argc, char* argv[]) {
    int exit_code = EXIT_SUCCESS;

    for (int i = 1; i < argc; i++) {
        cout << i << "\t" << argv[i] << endl;
        ifstream json_file(argv[i], ifstream::binary);
        BidirectedGraph g;
        cout << "Deserialization: " << (g.deserialize(json_file) ? "success" : "failure") << "!" << endl;
        auto bundles = find_balanced_bundles(g);
        for (auto bundle : bundles) {
            print_bundle(g, bundle);
        }
        cout << "Nodes: ";
        g.for_each_handle([&](const handle_t& handle) {
            cout << g.get_id(handle) << " ";
        });
        cout << endl;
#ifdef DEBUG_BIDIRECTED_GRAPH
        cout << "Edges:" << endl;
        print_edges(g);
#endif /* DEBUG_BIDIRECTED_GRAPH */
    }

    return exit_code;
}