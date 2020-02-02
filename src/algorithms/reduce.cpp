#include "reduce.hpp"
#include <unordered_map>
#include <vector>

using namespace handlegraph;
using namespace std;

using bundle_map_t = unordered_map<handle_t, Bundle*>;

//******************************************************************************
// Helper functions
//******************************************************************************

struct Global {
    static BundlePool* bpool;
};
BundlePool* Global::bpool = BundlePool::get_instance();

/// Returns the first neighbor of the node.
inline handle_t get_first_neighbor(HandleGraph& g, handle_t& node, bool is_left) {
    handle_t neighbor;
    g.follow_edges(node, is_left, [&](const handle_t& handle) { neighbor = handle; return false; });
    return neighbor;
}

/// Reduction action 1
void reduce_degree_one_node(DeletableHandleGraph& g, handle_t& node) {
    handle_t left_neighbor = get_first_neighbor(g, node, true);
    handle_t right_neighbor = get_first_neighbor(g, node, false);
    g.create_edge(left_neighbor, right_neighbor);

    g.destroy_handle(node);
}

/// Reduction action 2
handle_t reduce_trivial_bundle(DeletableHandleGraph& g, Bundle& bundle) {
    handle_t l_handle = *bundle.get_left().begin();
    handle_t r_handle = *bundle.get_right().begin();

    /// Create new node
    handle_t new_handle = g.create_handle("");

    /// Remap left side edges of lhandle to new node
    g.follow_edges(l_handle, true, [&](const handle_t& child_handle) {
        g.create_edge(child_handle, new_handle);
    });

    /// Remap right side edges of rhandle to new node
    g.follow_edges(r_handle, false, [&](const handle_t& child_handle) {
        g.create_edge(new_handle, child_handle);
    });

    g.destroy_handle(l_handle);
    g.destroy_handle(r_handle);
    
    return new_handle;
}

/// Reduction action 3
Bundle* reduce_bundle(DeletableHandleGraph& g, Bundle& bundle) {
    /// Create new nodes for left and right side
    handle_t newl_handle = g.create_handle("");
    handle_t newr_handle = g.create_handle("");
    
    /// Create edge between newl_handle and newr_handle
    g.create_edge(newl_handle, newr_handle);

    handle_t l_handle = *bundle.get_left().begin();
    handle_t r_handle = *bundle.get_right().begin();    

    /// Connect lhs neighbors to new left node
    g.follow_edges(l_handle, true, [&](const handle_t& lnei_handle) {
        g.create_edge(lnei_handle, newl_handle);
    });
    /// Connect new right node to rhs neighbors
    g.follow_edges(r_handle, false, [&](const handle_t& rnei_handle) {
        g.create_edge(newr_handle, rnei_handle);
    });

    /// Delete bundle's nodes
    for (const auto& node : bundle.get_left()) {
        g.destroy_handle(node);
    }
    for (const auto& node : bundle.get_right()) {
        g.destroy_handle(node);
    }

    Bundle* reduced_bundle = Global::bpool->get_bundle();
    (*reduced_bundle).get_left().add_init_node(newl_handle);
    (*reduced_bundle).get_right().add_init_node(newr_handle);
    return reduced_bundle;
}

/// Creates node->bundle mappings for each node in the bundle.
void mark_bundle(HandleGraph& g, Bundle* bundle, bundle_map_t& bundle_map) {
    for (auto& handle : bundle->get_left()) {
        bundle_map[handle] = bundle;
    }
    for (auto& handle : bundle->get_right()) {
        bundle_map[g.flip(handle)] = bundle;
    }
}

/// Removes node->bundle mappings for each node in the bundle and recycles it.
void unmark_bundle(HandleGraph& g, Bundle* bundle, bundle_map_t& bundle_map) {
    /// Go through each bundleside and remove their bundle reference
    for (auto& handle : bundle->get_left()) {
        bundle_map.erase(handle);
    }
    for (auto& handle : bundle->get_right()) {
        bundle_map.erase(g.flip(handle));
    }

    /// Return Bundle back to the pool
    Global::bpool->return_bundle(bundle);
}

/// Performs reduction action 1 where node is the removed node.
void perform_reduction1(DeletableHandleGraph& g, handle_t& node, bundle_map_t& bundle_map) {
    /// Get left neighbor so bundle finding could be done post node removal
    handle_t left_neighbor = get_first_neighbor(g, node, true);
    
    /// Delete node entry in bundle_map if any
    if (bundle_map.count(node)) unmark_bundle(g, bundle_map[node], bundle_map);
    if (bundle_map.count(g.flip(node))) unmark_bundle(g, bundle_map[g.flip(node)], bundle_map);
    
    /// Perform reduction action 1
    reduce_degree_one_node(g, node);

    /// See if this causes the left neighbor to be part of a new bundle, if so, update
    /// Only need to check left neighbor because if the right neighbor is part of some bundle
    /// that bundle has to be connected to the left neighbor (since there's now an edge
    /// between the two).
    auto [in_bundle, bundle] = find_balanced_bundle(left_neighbor, g);
    if (in_bundle) mark_bundle(g, bundle, bundle_map);
}

/// Performs reduction action 2 where bundle is the trivial bundle.
void perform_reduction2(DeletableHandleGraph& g, Bundle& bundle, bundle_map_t& bundle_map) {
    /// Perform reduction action 2
    handle_t node = reduce_trivial_bundle(g, bundle);

    /// Delete node entries in bundle
    handle_t left = *bundle.get_left().begin();
    handle_t right = *bundle.get_right().begin();
    unmark_bundle(g, &bundle, bundle_map);
    if (bundle_map.count(g.flip(left))) unmark_bundle(g, bundle_map[g.flip(left)], bundle_map); 
    if (bundle_map.count(right)) unmark_bundle(g, bundle_map[right], bundle_map);
    /// TODO: Should implement a method to rename a node in a bundle (saves having to
    /// recompute a bundle that's simply has one node that's been renamed).

    /// Try to reinitialize bundles (if any)
    auto [in_bundle1, bundle1] = find_balanced_bundle(g.flip(node), g);
    if (in_bundle1) { mark_bundle(g, bundle1, bundle_map); }
    auto [in_bundle2, bundle2] = find_balanced_bundle(node, g);
    if (in_bundle2) { mark_bundle(g, bundle2, bundle_map); }
}

/// Performs reduction action 3 where bundle is the middle bundle.
void perform_reduction3(DeletableHandleGraph& g, Bundle& bundle, bundle_map_t& bundle_map) {
    /// Reduce bundle
    Bundle* reduced_bundle = reduce_bundle(g, bundle);

    /// Remove references of the left of L nodes and right of R nodes (must be 
    /// strongly adjacent to other bundles or this reduction action won't be
    /// performed)
    handle_t left_node = *bundle.get_left().begin();
    unmark_bundle(g, bundle_map[g.flip(left_node)], bundle_map);
    handle_t right_node = *bundle.get_right().begin();
    unmark_bundle(g, bundle_map[right_node], bundle_map);
    unmark_bundle(g, &bundle, bundle_map);

    /// Get new left/right nodes for the bundle
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

/// Returns middle node's handle as return_handle
bool is_reduction1(const HandleGraph& g, const nid_t& node_id, handle_t& return_handle) {
    handle_t node = g.get_handle(node_id);
    if (g.get_degree(node, false) == 1 && g.get_degree(node, true) == 1) {
        return_handle = node;
        return true;
    }
    return false;
}

/// Returns direction of node_id (for the trivial bundle) as return_handle
bool is_reduction2(const HandleGraph& g, const nid_t& node_id, bundle_map_t& bundle_map, handle_t& return_handle) {
    handle_t node = g.get_handle(node_id);
    if (bundle_map.count(node) && bundle_map[node]->is_trivial()) {
        return_handle = node;
        return true;
    }
    if (bundle_map.count(g.flip(node)) && bundle_map[g.flip(node)]->is_trivial()) {
        return_handle = g.flip(node);
        return true;
    }
    return false;
}

/// Returns the middle bundle as return_handle
bool is_reduction3(const HandleGraph& g, const nid_t& node_id, bundle_map_t& bundle_map, handle_t& return_handle) {
    function<bool(const handle_t& handle)> is_valid_reduction = [&](const handle_t& node) {
        /// Check conditions for the middle bundle
        if (!bundle_map.count(node)) return false; /// There is no bundle
        Bundle* mid_b = bundle_map[node];
        if (mid_b->is_trivial()) return false; /// Ignore trivial middle bundles

        /// Check conditions for the "left" bundle
        if (!bundle_map.count(g.flip(node))) return false; /// Nothing on the "left"
        Bundle* left_b = bundle_map[g.flip(node)];
        if (mid_b->get_adjacency_type(*left_b) != adjacency_t::Strong) return false;

        /// Check conditions for the "right" bundle
        handle_t node_ = (mid_b->is_reversed(node)) ? g.flip(node) : node; /// The correct orientation to iterate through the bundle
        handle_t node_mid_r; /// Some node that's on the opposite side of node in the middle bundle
        mid_b->traverse_bundle(node_, [&](const handle_t& handle) { node_mid_r = handle; return false; });
        if (!bundle_map.count(node_mid_r)) return false;                
        Bundle* right_b = bundle_map[node_mid_r];
        if (mid_b->get_adjacency_type(*right_b) != adjacency_t::Strong) return false; 

        return_handle = node;
        return true;
    };

    handle_t node = g.get_handle(node_id);
    if (is_valid_reduction(node)) {
        return true;
    }
    if (is_valid_reduction(g.flip(node))) {
        return true;
    }
    
    return false;
}

/// Returns true if the final graph is trivial
void reduce_graph(DeletableHandleGraph& g) {
    // unordered_set<nid_t> updated;
    // g.for_each_handle([&](const handle_t& handle) { updated.insert(g.get_id(handle)); });
    bundle_map_t bundle_map;

    // while (!updated.empty()) {
    bool updated = true;                                                // Unoptimized
    while (updated) {                                                   // Unoptimized
        // unordered_set<nid_t> updated_new;
        updated = false;

        // for (auto& node_id : updated) {
        g.for_each_handle([&](const handle_t& handle) {                 // Unoptimized
            nid_t node_id = g.get_id(handle);                           // Unoptimized

            /// Check if handle is in the graph
            // if (!g.has_node(node_id)) continue;
            if (!g.has_node(node_id)) return;                           // Unoptimized

            handle_t return_handle;
            /// If node is in graph, check conditions for RA1, RA3, then RA2
            if (is_reduction1(g, node_id, return_handle)) {

            } else if (is_reduction3(g, node_id, bundle_map, return_handle)) {

            } else if (is_reduction2(g, node_id, bundle_map, return_handle)) {

            }
        }
        );                                                              // Unoptimized

        // updated = updated_new;
    }
}