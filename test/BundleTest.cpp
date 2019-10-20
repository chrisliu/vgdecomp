#include <iostream>

#include "../src/BidirectedGraph.hpp"
#include "../src/BidirectedGraphBuilder.hpp"
#include "../src/algorithms/find_bundles.hpp"
#include "../src/algorithms/bundle.hpp"

using namespace std;

void print_bundle(BidirectedGraph* g, Bundle& bundle) {
    cout << "Left" << endl;
    bundle.get_bundleside(true).traverse_bundle([&](const handle_t& l_handle) {
        cout << g->get_id(l_handle) << endl;
        return true;
    });
    cout << "Right" << endl;
    bundle.get_bundleside(false).traverse_bundle([&](const handle_t& r_handle) {
        cout << g->get_id(r_handle) << endl;
        return true;
    }); 
}

/// Usage:
/// Call BundleTest binary with the test graph json files as the arguments
/// Ex: ./BundleTest test/bundle_test_graph/00_trivial.json
int main(int argc, char* argv[]) {
    int exit_code = EXIT_SUCCESS;

    for (int i = 1; i < argc; i++) {
        cout << i << "\t" << argv[i] << endl;
        BidirectedGraph g = BidirectedGraphBuilder::build_graph(argv[i]);
    }

    return exit_code;
}