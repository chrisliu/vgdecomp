#define CATCH_CONFIG_RUNNER
#include "../catch2/catch.hpp"
#include "../../src/BidirectedGraph.hpp"
#include "../../src/algorithms/bundle.hpp"

Bundle bundle1, bundle1_reversed, bundle2, bundle3, bundle3_reversed;

/// Parameters:
///    b1: A Bundle
///    b2: Another bundle
///    adjs: A list of Adjacencies arranged such that the values are
///        the adjacencies of b1L b2L, b1L b2R, b1R b2L, b1R b2R
void test_bundle_adjacencies(Bundle b1, Bundle b2, Adjacency adjs[]) {
    Adjacency adj;
    adj = b1.get_bundleside(true).get_adjacency(b2.get_bundleside(true));
    REQUIRE ( adj == adjs[0] );
    adj = b1.get_bundleside(true).get_adjacency(b2.get_bundleside(false));
    REQUIRE ( adj == adjs[1] );
    adj = b1.get_bundleside(false).get_adjacency(b2.get_bundleside(true));
    REQUIRE ( adj == adjs[2] );
    adj = b1.get_bundleside(false).get_adjacency(b2.get_bundleside(false));
    REQUIRE ( adj == adjs[3] );
}

void inline test_identity(Bundle b1, Bundle b2) {
    Adjacency adjs[] = {Adjacency::Strong, Adjacency::None, Adjacency::None, Adjacency::Strong}; 
    test_bundle_adjacencies(b1, b2, adjs);
}

void inline test_identity(Bundle b) {
    test_identity(b, b);
}

/// Bundle 1
TEST_CASE ( "Bundle 1 and Bundle 1" ) {
    test_identity(bundle1);
}

TEST_CASE ( "Bundle 1 and Bundle 1 Reversed" ) {
    test_identity(bundle1, bundle1_reversed);
}

TEST_CASE ( "Bundle 1 and Bundle 2" ) {
    Adjacency adjs[] = {Adjacency::None, Adjacency::None, Adjacency::Weak, Adjacency::None}; 
    test_bundle_adjacencies(bundle1, bundle2, adjs);
}

TEST_CASE ( "Bundle 1 and Bundle 3" ) {
    Adjacency adjs[] = {Adjacency::None, Adjacency::None, Adjacency::None, Adjacency::None}; 
    test_bundle_adjacencies(bundle1, bundle3, adjs);
}

TEST_CASE ( "Bundle 1 and Bundle 3 Reversed" ) {
    Adjacency adjs[] = {Adjacency::None, Adjacency::None, Adjacency::None, Adjacency::None}; 
    test_bundle_adjacencies(bundle1, bundle3_reversed, adjs);
}

/// Bundle 1 Reversed
TEST_CASE ( "Bundle 1 Reversed and Bundle 1" ) {
    test_identity(bundle1_reversed, bundle1);
}

TEST_CASE ( "Bundle 1 Reversed and Bundle 1 Reversed" ) {
    test_identity(bundle1_reversed);
}

TEST_CASE ( "Bundle 1 Reversed and Bundle 2" ) {
    Adjacency adjs[] = {Adjacency::None, Adjacency::None, Adjacency::Weak, Adjacency::None}; 
    test_bundle_adjacencies(bundle1_reversed, bundle2, adjs);
}

TEST_CASE ( "Bundle 1 Reversed and Bundle 3" ) {
    Adjacency adjs[] = {Adjacency::None, Adjacency::None, Adjacency::None, Adjacency::None}; 
    test_bundle_adjacencies(bundle1_reversed, bundle3, adjs);
}

TEST_CASE ( "Bundle 1 Reversed and Bundle 3 Reversed" ) {
    Adjacency adjs[] = {Adjacency::None, Adjacency::None, Adjacency::None, Adjacency::None}; 
    test_bundle_adjacencies(bundle1_reversed, bundle3_reversed, adjs);
}

/// Bundle 2
TEST_CASE ( "Bundle 2 and Bundle 1" ) {
    Adjacency adjs[] = {Adjacency::None, Adjacency::Weak, Adjacency::None, Adjacency::None}; 
    test_bundle_adjacencies(bundle2, bundle1, adjs);
}

TEST_CASE ( "Bundle 2 and Bundle 1 Reversed" ) {
    Adjacency adjs[] = {Adjacency::None, Adjacency::Weak, Adjacency::None, Adjacency::None}; 
    test_bundle_adjacencies(bundle2, bundle1_reversed, adjs);
}

TEST_CASE ( "Bundle 2 and Bundle 2" ) {
    test_identity(bundle2);
}

TEST_CASE ( "Bundle 2 and Bundle 3" ) {
    Adjacency adjs[] = {Adjacency::None, Adjacency::None, Adjacency::Strong, Adjacency::None}; 
    test_bundle_adjacencies(bundle2, bundle3, adjs);
}

TEST_CASE ( "Bundle 2 and Bundle 3 Reversed" ) {
    Adjacency adjs[] = {Adjacency::None, Adjacency::None, Adjacency::Strong, Adjacency::None}; 
    test_bundle_adjacencies(bundle2, bundle3_reversed, adjs);
}

/// Bundle 3
TEST_CASE ( "Bundle 3 and Bundle 1" ) {
    Adjacency adjs[] = {Adjacency::None, Adjacency::None, Adjacency::None, Adjacency::None}; 
    test_bundle_adjacencies(bundle3, bundle1, adjs);
}

TEST_CASE ( "Bundle 3 and Bundle 1 Reversed" ) {
    Adjacency adjs[] = {Adjacency::None, Adjacency::None, Adjacency::None, Adjacency::None}; 
    test_bundle_adjacencies(bundle3, bundle1_reversed, adjs);
}

TEST_CASE ( "Bundle 3 and Bundle 2" ) {
    Adjacency adjs[] = {Adjacency::None, Adjacency::Strong, Adjacency::None, Adjacency::None}; 
    test_bundle_adjacencies(bundle3, bundle2, adjs);
}

TEST_CASE ( "Bundle 3 and Bundle 3" ) {
    test_identity(bundle3);
}

TEST_CASE ( "Bundle 3 and Bundle 3 Reversed" ) {
    test_identity(bundle3, bundle3_reversed);
}

/// Bundle 3 Reversed
TEST_CASE ( "Bundle 3 Reversed and Bundle 1" ) {
    Adjacency adjs[] = {Adjacency::None, Adjacency::None, Adjacency::None, Adjacency::None}; 
    test_bundle_adjacencies(bundle3_reversed, bundle1, adjs);
}

TEST_CASE ( "Bundle 3 Reversed and Bundle 1 Reversed" ) {
    Adjacency adjs[] = {Adjacency::None, Adjacency::None, Adjacency::None, Adjacency::None}; 
    test_bundle_adjacencies(bundle3_reversed, bundle1_reversed, adjs);
}

TEST_CASE ( "Bundle 3 Reversed and Bundle 2" ) {
    Adjacency adjs[] = {Adjacency::None, Adjacency::Strong, Adjacency::None, Adjacency::None}; 
    test_bundle_adjacencies(bundle3_reversed, bundle2, adjs);
}

TEST_CASE ( "Bundle 3 Reversed and Bundle 3" ) {
    test_identity(bundle3_reversed, bundle3);
}

TEST_CASE ( "Bundle 3 Reversed and Bundle 3 Reversed" ) {
    test_identity(bundle3_reversed);
}

int main (int argc, char* argv[]) {
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
    g.add_edge (1, 3, false, false);
    g.add_edge (2, 3, false, false);
    g.add_edge (3, 5, false, false);
    g.add_edge (3, 6, false, false);
    g.add_edge (4, 5, false, false);
    g.add_edge (4, 6, false, false);
    g.add_edge (5, 7, false, false);
    g.add_edge (6, 7, false, false);
 
    /// Create bundles
    bundle1.add_node (g.get_handle(1), true);
    bundle1.add_node (g.get_handle(2), true);
    bundle1.add_node (g.get_handle(3), false);
    bundle1.freeze (g);
    
    bundle1_reversed.add_node (g.get_handle(1, true), true);
    bundle1_reversed.add_node (g.get_handle(2, true), true);
    bundle1_reversed.add_node (g.get_handle(3, true), false);
    bundle1_reversed.freeze (g);

    bundle2.add_node (g.get_handle(3), true);
    bundle2.add_node (g.get_handle(4), true);
    bundle2.add_node (g.get_handle(5), false);
    bundle2.add_node (g.get_handle(6), false);
    bundle2.freeze (g);

    bundle3.add_node (g.get_handle(5), true);
    bundle3.add_node (g.get_handle(6), true);
    bundle3.add_node (g.get_handle(7), false); 
    bundle3.freeze (g);

    bundle3_reversed.add_node (g.get_handle(5, true), true);
    bundle3_reversed.add_node (g.get_handle(6, true), true);
    bundle3_reversed.add_node (g.get_handle(7, true), false); 
    bundle3_reversed.freeze (g);

    return Catch::Session().run(argc, argv);
}