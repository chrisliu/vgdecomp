#include <iostream>
#include <sstream>
#include <string>
#include "../../src/BidirectedGraph.hpp"
#include "../../src/algorithms/bundle.hpp"

using namespace std;

inline string adjacency_to_string(Adjacency t) {
    switch (t) {
        case Adjacency::None:
            return "None";
        case Adjacency::Weak:
            return "Weak";
        case Adjacency::Strong:
            return "Strong";
    }
    return "";
}

string adjacency_debug_str(const BundleSide& side1, const BundleSide& side2, 
    const string descr1, const string descr2
) {
    stringstream ss;
    ss << "Adjacency between " << descr1 << " and " << descr2 << " is " << \
        adjacency_to_string(side1.get_adjacency(side2)) << ".";
    return ss.str();
} 

int main(int argc, char* argv[]) {
    /// Construct Bidirected Graph
    /// 1 -|
    ///    -- 3 --- 5 -|
    /// 2 -|     x     -- 7
    ///       4 --- 6 -|
    /// Where the bundles are:
    ///   Bundle 1: L (1, 2) R (3)
    ///   Bundle 2: L (3, 4) R (5, 6)
    ///   Bundle 3: L (5, 6) R (7)
    /// Such that the adjacencies are:
    ///   Adjacency 1: B1R (weak)   B2L
    ///   Adjacency 2: B2R (strong) B3L

    BidirectedGraph g;
    g.add_edge(1, 3, false, false);
    g.add_edge(2, 3, false, false);
    g.add_edge(3, 5, false, false);
    g.add_edge(3, 6, false, false);
    g.add_edge(4, 5, false, false);
    g.add_edge(4, 6, false, false);
    g.add_edge(5, 7, false, false);
    g.add_edge(6, 7, false, false);

    /// Create bundles
    Bundle bundle1;
    bundle1.add_node(g.get_handle(1), true);
    bundle1.add_node(g.get_handle(2), true);
    bundle1.add_node(g.get_handle(3), false);
    bundle1.freeze(g);
    
    Bundle bundle1_reversed;
    bundle1_reversed.add_node(g.get_handle(1, true), true);
    bundle1_reversed.add_node(g.get_handle(2, true), true);
    bundle1_reversed.add_node(g.get_handle(3, true), false);
    bundle1_reversed.freeze(g);

    Bundle bundle2;
    bundle2.add_node(g.get_handle(3), true);
    bundle2.add_node(g.get_handle(4), true);
    bundle2.add_node(g.get_handle(5), false);
    bundle2.add_node(g.get_handle(6), false);
    bundle2.freeze(g);

    Bundle bundle3;
    bundle3.add_node(g.get_handle(5), true);
    bundle3.add_node(g.get_handle(6), true);
    bundle3.add_node(g.get_handle(7), false); 
    bundle3.freeze(g);

    Bundle bundle3_reversed;
    bundle3_reversed.add_node(g.get_handle(5, true), true);
    bundle3_reversed.add_node(g.get_handle(6, true), true);
    bundle3_reversed.add_node(g.get_handle(7, true), false); 
    bundle3_reversed.freeze(g);

    /// Check nodeside adjacency
    cout << adjacency_debug_str(bundle1.get_bundleside(false), bundle2.get_bundleside(true),
        "B1R", "B2L") << endl;
    cout << adjacency_debug_str(bundle2.get_bundleside(false), bundle3.get_bundleside(true),
        "B2L", "B3L") << endl;
    cout << adjacency_debug_str(bundle1_reversed.get_bundleside(false), 
        bundle2.get_bundleside(true), "B1RR", "B2L") << endl;
    cout << adjacency_debug_str(bundle2.get_bundleside(false),
        bundle3_reversed.get_bundleside(true), "B2R", "B3RL") << endl;;
    cout << adjacency_debug_str(bundle1.get_bundleside(true), bundle3.get_bundleside(false),
        "B1L", "B3R") << endl;;

    return EXIT_SUCCESS;
}