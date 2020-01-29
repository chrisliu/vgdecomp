#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <assert.h>

#include "../../src/BidirectedGraph.hpp"
#include "../../src/algorithms/find_balanced_bundles.hpp"
#include "../../src/algorithms/bundle.hpp"
#include "../../src/algorithms/reduce.hpp"
#include "../../deps/handlegraph/types.hpp"

using namespace std;
using namespace handlegraph;

using bundle_map_t = unordered_map<handle_t, Bundle*>;

inline void mark_bundle(HandleGraph& g, Bundle* bundle, bundle_map_t& bundle_map) {
    for (auto& handle : bundle->get_left()) {
        bundle_map[handle] = bundle;
    }
    for (auto& handle : bundle->get_right()) {
        bundle_map[g.flip(handle)] = bundle;
    }
}

void perform_reduction1(DeletableHandleGraph& g, handle_t& node, bundle_map_t& bundle_map) {
    /// Get left neighbor so bundle finding could be done post node removal
    handle_t left_neighbor;
    g.follow_edges(node, true, [&](const handle_t& handle) { left_neighbor = handle; });
    
    /// Delete node entry in bundle_map if any
    if (bundle_map.count(node)) bundle_map.erase(node);
    if (bundle_map.count(g.flip(node))) bundle_map.erase(g.flip(node));
    /// TODO: Delete bundle and remove node entry in other bundles
    
    /// Perform reduction action 1
    reduce_degree_one_node(g, node);

    /// See if this causes the left neighbor to be part of a new bundle, if so, update
    auto [in_bundle, bundle] = find_balanced_bundle(left_neighbor, g);
    if (in_bundle) { mark_bundle(g, bundle, bundle_map); }
}

void perform_reduction2(DeletableHandleGraph& g, Bundle& bundle, bundle_map_t& bundle_map) {
    /// Delete node entries in bundle
    handle_t left = *bundle.get_left().begin();
    handle_t right = *bundle.get_right().begin();
    bundle_map.erase(left);
    bundle_map.erase(g.flip(right));
    if (bundle_map.count(g.flip(left))) bundle_map.erase(g.flip(left));
    if (bundle_map.count(right)) bundle_map.erase(right);
    /// TODO: Delete bundles

    /// Perform reduction action 2
    handle_t node = reduce_trivial_bundle(g, bundle);

    /// Try to reinitialize bundles (if any)
    auto [in_bundle1, bundle1] = find_balanced_bundle(g.flip(node), g);
    if (in_bundle1) { mark_bundle(g, bundle1, bundle_map); }
    auto [in_bundle2, bundle2] = find_balanced_bundle(node, g);
    if (in_bundle2) { mark_bundle(g, bundle2, bundle_map); }
}

void perform_reduction3(DeletableHandleGraph& g, Bundle& bundle, bundle_map_t& bundle_map) {
    /// Remove references of the left of L nodes and right of R nodes (must be 
    /// strongly adjacent to other bundles or this reduction action won't be
    /// performed)
    for (auto& handle : bundle.get_left()) { 
        bundle_map.erase(handle);
        if (bundle_map.count(g.flip(handle))) bundle_map.erase(g.flip(handle));
    }
    for (auto& handle : bundle.get_right()) {
        bundle_map.erase(g.flip(handle)); 
        if (bundle_map.count(handle)) bundle_map.erase(handle);
    }
    /// TODO: Delete bundle

    /// Reduce bundle
    Bundle* reduced_bundle = reduce_bundle(g, bundle);
    handle_t node1 = *reduced_bundle->get_left().begin();
    handle_t node2 = *reduced_bundle->get_right().begin();

    /// Add bundle created by new nodes
    mark_bundle(g, reduced_bundle, bundle_map);

    /// Try to find new bundles from the left side of node1 and right side of node2
    auto [in_bundle1, bundle1] = find_balanced_bundle(g.flip(node1), g);
    if (in_bundle1) { mark_bundle(g, bundle1, bundle_map); }
    auto [in_bundle2, bundle2] = find_balanced_bundle(node2, g);
    if (in_bundle2) { mark_bundle(g, bundle2, bundle_map); }
}

/// Forward declarations for verification steps
inline void verify_init_bundles(const HandleGraph& g, const bundle_map_t& bundle_map);
inline void verify_step1_bundles(const HandleGraph& g, const bundle_map_t& bundle_map);
inline void verify_step2_bundles(const HandleGraph& g, const bundle_map_t& bundle_map);
inline void verify_step3_bundles(const HandleGraph& g, const bundle_map_t& bundle_map);
inline void verify_step4_bundles(const HandleGraph& g, const bundle_map_t& bundle_map);
inline void verify_step5_bundles(const HandleGraph& g, const bundle_map_t& bundle_map);

int main(int argc, char* argv[]) {
    /// Load graph
    string filename = "email_graph.json";
    ifstream json_file(filename, ifstream::binary);
    BidirectedGraph g;
    g.deserialize(json_file); 

    /// Find and map all bundles
    vector<Bundle*> bundles = find_balanced_bundles(g);
    bundle_map_t bundle_map; /// Stores associated bundles in forward direction (go_left is false what is the bundle it enters)
    for (auto& bundle : bundles) { mark_bundle(g, bundle, bundle_map); }

    /// Verify bundles are being marked correctly
    verify_init_bundles(g, bundle_map);

    /// Perform reduction actions as described by the email
    /// Reduce <2>-<4>-<5> => <2>-<5> with R1
    cout << "=== Step 1 ===" << endl;
    handle_t node4 = g.get_handle(4);
    perform_reduction1(g, node4, bundle_map);
    verify_step1_bundles(g, bundle_map);

    cout << g.get_id(*(*bundle_map[g.get_handle(2)]).get_left().begin()) << endl;

    /// Reduce     /-<2>-<5>-\
    ///        <1>-     x     -<7> => <1>-<8>-<9>-<7>
    ///            \-<3>-<6>-/
    cout << "=== Step 2 ===" << endl;
    perform_reduction3(g, *bundle_map[g.get_handle(2)], bundle_map);
    verify_step2_bundles(g, bundle_map);

    /// Reduce <1>-<8>-<9>-<7> => <1>-<9>-<7>
    cout << "=== Step 3 ===" << endl;
    g.follow_edges(g.get_handle(9), false, [&](const handle_t& handle) {
        cout << "Node " << g.get_id(handle) << endl;
    });
    handle_t node8 = g.get_handle(8);
    perform_reduction1(g, node8, bundle_map);
    verify_step3_bundles(g, bundle_map);

    /// Reduce <1>-<9>-<7> => <1>-<7>
    cout << "=== Step 4 ===" << endl;
    g.follow_edges(g.get_handle(9), false, [&](const handle_t& handle) {
        cout << "Node " << g.get_id(handle) << endl;
    });
    handle_t node9 = g.get_handle(9);
    perform_reduction1(g, node9, bundle_map);
    verify_step4_bundles(g, bundle_map);

    /// Reduce <1>-<7> => <10>
    cout << "=== Step 5 ===" << endl;
    perform_reduction2(g, *bundle_map[g.get_handle(1)], bundle_map);
    verify_step5_bundles(g, bundle_map);

    /// Print graph
    cout << "=== Graph ===" << endl;
    g.for_each_handle([&](const handle_t& handle) {
        cout << "Node " << g.get_id(handle) << endl;
        g.follow_edges(handle, true, [&](const handle_t& lhandle) {
            cout << "(L) Node " << g.get_id(lhandle) << endl; 
        });
        g.follow_edges(handle, false, [&](const handle_t& rhandle) {
            cout << "(R) Node " << g.get_id(rhandle) << endl;
        });
    });

    return EXIT_SUCCESS;
}

inline void verify_init_bundles(const HandleGraph& g, const bundle_map_t& bundle_map) {
    assert(bundle_map.count(g.get_handle(1))); // Bundle exists
    assert(!bundle_map.count(g.get_handle(1, true))); // Bundle doesn't exist
    assert(!bundle_map.count(g.get_handle(2)));
    assert(bundle_map.count(g.get_handle(2, true)));
    assert(!bundle_map.count(g.get_handle(3)));
    assert(bundle_map.count(g.get_handle(3, true)));
    assert(!bundle_map.count(g.get_handle(4)));
    assert(!bundle_map.count(g.get_handle(4, true)));
    assert(bundle_map.count(g.get_handle(5)));
    assert(!bundle_map.count(g.get_handle(5, true)));
    assert(bundle_map.count(g.get_handle(6)));
    assert(!bundle_map.count(g.get_handle(6, true)));
    assert(!bundle_map.count(g.get_handle(7)));
    assert(bundle_map.count(g.get_handle(7, true)));
}

inline void verify_step1_bundles(const HandleGraph& g, const bundle_map_t& bundle_map) {
    assert(bundle_map.count(g.get_handle(1)));
    assert(!bundle_map.count(g.get_handle(1, true)));
    assert(bundle_map.count(g.get_handle(2)));
    assert(bundle_map.count(g.get_handle(2, true)));
    assert(bundle_map.count(g.get_handle(3)));
    assert(bundle_map.count(g.get_handle(3, true)));
    assert(!bundle_map.count(g.get_handle(4))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(4, true))); // Doesn't exist
    assert(bundle_map.count(g.get_handle(5)));
    assert(bundle_map.count(g.get_handle(5, true)));
    assert(bundle_map.count(g.get_handle(6)));
    assert(bundle_map.count(g.get_handle(6, true)));
    assert(!bundle_map.count(g.get_handle(7)));
    assert(bundle_map.count(g.get_handle(7, true)));
}

inline void verify_step2_bundles(const HandleGraph& g, const bundle_map_t& bundle_map) {
    assert(bundle_map.count(g.get_handle(1)));
    assert(!bundle_map.count(g.get_handle(1, true)));
    assert(!bundle_map.count(g.get_handle(2))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(2, true))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(3))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(3, true))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(4))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(4, true))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(5))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(5, true))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(6))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(6, true))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(7)));
    assert(bundle_map.count(g.get_handle(7, true)));
    assert(bundle_map.count(g.get_handle(8))); // New node
    assert(bundle_map.count(g.get_handle(8, true))); // New node
    assert(bundle_map.count(g.get_handle(9))); // New node
    assert(bundle_map.count(g.get_handle(9, true))); // New node
}

inline void verify_step3_bundles(const HandleGraph& g, const bundle_map_t& bundle_map) {
    assert(bundle_map.count(g.get_handle(1)));
    assert(!bundle_map.count(g.get_handle(1, true)));
    assert(!bundle_map.count(g.get_handle(2))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(2, true))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(3))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(3, true))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(4))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(4, true))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(5))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(5, true))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(6))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(6, true))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(7)));
    assert(bundle_map.count(g.get_handle(7, true)));
    assert(!bundle_map.count(g.get_handle(8))); // Doesn't exist 
    assert(!bundle_map.count(g.get_handle(8, true))); // Doesn't exist
    assert(bundle_map.count(g.get_handle(9)));
    assert(bundle_map.count(g.get_handle(9, true)));
}

inline void verify_step4_bundles(const HandleGraph& g, const bundle_map_t& bundle_map) {
    assert(bundle_map.count(g.get_handle(1)));
    assert(!bundle_map.count(g.get_handle(1, true)));
    assert(!bundle_map.count(g.get_handle(2))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(2, true))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(3))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(3, true))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(4))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(4, true))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(5))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(5, true))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(6))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(6, true))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(7)));
    assert(bundle_map.count(g.get_handle(7, true)));
    assert(!bundle_map.count(g.get_handle(8))); // Doesn't exist 
    assert(!bundle_map.count(g.get_handle(8, true))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(9))); // Doesn't exist 
    assert(!bundle_map.count(g.get_handle(9, true))); // Doesn't exist 
}

inline void verify_step5_bundles(const HandleGraph& g, const bundle_map_t& bundle_map) {
    assert(!bundle_map.count(g.get_handle(1))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(1, true))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(2))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(2, true))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(3))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(3, true))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(4))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(4, true))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(5))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(5, true))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(6))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(6, true))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(7))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(7, true))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(8))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(8, true))); // Doesn't exist
    assert(!bundle_map.count(g.get_handle(9))); // Doesn't exist 
    assert(!bundle_map.count(g.get_handle(9, true))); // Doesn't exist 
    assert(!bundle_map.count(g.get_handle(10)));
    assert(!bundle_map.count(g.get_handle(10, true)));
}