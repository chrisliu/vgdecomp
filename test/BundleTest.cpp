#include <iostream>

#include "../src/BidirectedGraph.hpp"
#include "../src/algorithms/find_bundles.hpp"

using namespace std;

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
    cout << bundles.size() << endl;    

    return exit_code;
}