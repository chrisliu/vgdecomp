#include <iostream>

#include "../src/BidirectedGraph.hpp"
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

int main(int argc, char* argv[]) {
    int exit_code = EXIT_SUCCESS;

    // Basic graph
    //    -- 2
    // 1 -|
    //    -- 3

    BidirectedGraph g;
    g.add_edge(1, 2, false, false);
    g.add_edge(1, 3, false, false);

    auto bundles = find_bundles(&g);

    // Print information about the found bundle
    cout << "Bundle count: " << bundles.size() << endl;    
    for (int i = 0; i < bundles.size(); i++) {
        cout << "Bundle " << i + 1 << endl;
        print_bundle(&g, *(bundles.begin() + i));
    }

    return exit_code;
}