#include "reduce.hpp"
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "find_bundles.hpp"
#include "wang_hash.hpp"
#include "../../deps/handlegraph/util.hpp"

#ifdef DEBUG_REDUCE
#include <iostream> 
void print_node(const HandleGraph& g, const handle_t& handle) {
    std::cout << "(Node " <<  g.get_id(handle) << (g.get_is_reverse(handle) ? "r" : "") << ") ";
}
using namespace std;
#endif /* DEBUG_REDUCE */

using namespace handlegraph;
using namespace std;

using bundle_map_t = unordered_map<handle_t, Bundle*>;

//******************************************************************************
// Custom hashmap
//******************************************************************************
using handle_set_t = unordered_set<handle_t>;
struct handle_set_t_hash_fn {
    size_t operator()(const handle_set_t& set) const {
        size_t out = 0;
        for (const handle_t& handle: set) {
            out ^= vg::wang_hash_64(as_integer(handle));
        }
        return out;
    }
};
template<typename T>
using handle_set_map_t = unordered_map<handle_set_t, T, handle_set_t_hash_fn>;

//******************************************************************************
// Update structure
//******************************************************************************

struct node_update_t {
    unordered_set<handle_t> updated;

    handle_t get_updated() {
        handle_t first = *updated.begin();
        updated.erase(updated.begin());
        return first;
    }

    bool is_action_avail() {
        return updated.size();
    }
};

//******************************************************************************
// Helper functions
//******************************************************************************

struct Global {

    static BundlePool* bpool;
};
BundlePool* Global::bpool = BundlePool::get_instance();

/// Returns the first neighbor of the node.
inline handle_t get_first_neighbor(const HandleGraph& g, const handle_t& node, bool is_left) {
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

    if (g.get_id(l_handle) == g.get_id(r_handle)) {
#ifdef DEBUG_REDUCE
        cout << "\033[31mDeleting self cycle\033[0m" << endl;
#endif /* DEBUG_REDUCE */
        /// Only add neighbors if it's an improper cycle
        if (l_handle != r_handle) {
            g.follow_edges(l_handle, true, [&](const handle_t& l_nei) { g.create_edge(l_nei, new_handle); });
        }
        g.destroy_handle(l_handle);
        return new_handle;
    }

    /// Remap left side edges of lhandle to new node
    g.follow_edges(l_handle, true, [&](const handle_t& l_nei) { g.create_edge(l_nei, new_handle); });

    /// Remap right side edges of rhandle to new node
    g.follow_edges(r_handle, false, [&](const handle_t& r_nei) { g.create_edge(new_handle, r_nei); });

    g.destroy_handle(l_handle);
    g.destroy_handle(r_handle);
    
    return new_handle;
}

/// Reduction action 3
/// Handles in the nid_set_t will be oriented such that follow_edges with go_left = false
/// will go to nodes of the other bundleside
handle_t reduce_orbit(DeletableHandleGraph& g, handle_set_t& orbit) {
    /// Create new handle for retracted node
    handle_t new_handle = g.create_handle("");

    /// Attach all left negihbors (orbits means they are the same)
    g.follow_edges(*orbit.begin(), true, [&](const handle_t& l_nei) { g.create_edge(l_nei, new_handle); });

    /// Connect node to the right neighbors of all nodes in the orbit; destroy orbit node afterwards
    for (auto& o_handle : orbit) {
        g.follow_edges(o_handle, false, [&](const handle_t& r_nei) { g.create_edge(new_handle, r_nei); });  
        g.destroy_handle(o_handle); /// Destroy orbit handle
    }  

    return new_handle;
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
void perform_reduction1(DeletableHandleGraph& g, handle_t& node, bundle_map_t& bundle_map, node_update_t& updates) {
    /// Get left neighbor so bundle finding could be done post node removal
    handle_t left_neighbor = get_first_neighbor(g, node, true);
    handle_t right_neighbor = get_first_neighbor(g, node, false);
    
    /// Delete node entry in bundle_map if any
    /// Only when left_nei-node and/or node-right_nei are trivial bundles
    if (bundle_map.count(node)) unmark_bundle(g, bundle_map[node], bundle_map);
    if (bundle_map.count(g.flip(node))) unmark_bundle(g, bundle_map[g.flip(node)], bundle_map);
    
    /// Perform reduction action 1
    reduce_degree_one_node(g, node);

    /// See if this causes the left neighbor to be part of a new bundle, if so, update
    /// Only need to check left neighbor because if the right neighbor is part of some bundle
    /// that bundle has to be connected to the left neighbor (since there's now an edge
    /// between the two).
    auto [in_bundle, bundle] = find_bundle(left_neighbor, g);
    if (in_bundle) mark_bundle(g, bundle, bundle_map);

    /// The left neighbor and right neighbor have been updated
    /// Both handles point towards where node used to be
    updates.updated.insert(left_neighbor);
    updates.updated.insert(g.flip(right_neighbor));
}

inline void update_bundle_nodes(const HandleGraph& g, Bundle& bundle, node_update_t& updates) {
        // TODO: Rationalize if adding all nodes in the new bundle is necessary
        for (auto& l_node : bundle.get_left()) updates.updated.insert(l_node);
        for (auto& r_node : bundle.get_right()) updates.updated.insert(g.flip(r_node));
}

/// Performs reduction action 2 where bundle is the trivial bundle.
void perform_reduction2(DeletableHandleGraph& g, Bundle& bundle, bundle_map_t& bundle_map, node_update_t& updates) {
    /// Perform reduction action 2
    handle_t node = reduce_trivial_bundle(g, bundle);

    /// Delete node entries in bundle
    handle_t left = *bundle.get_left().begin();
    handle_t right = *bundle.get_right().begin();
    unmark_bundle(g, &bundle, bundle_map);
    if (bundle_map.count(g.flip(left))) unmark_bundle(g, bundle_map[g.flip(left)], bundle_map); 
    if (bundle_map.count(right)) unmark_bundle(g, bundle_map[right], bundle_map);
    // TODO: Should implement a method to rename a node in a bundle (saves having to
    /// recompute a bundle that's simply has one node that's been renamed).

    /// Try to reinitialize bundles (if any)
    auto [in_bundle1, bundle1] = find_bundle(g.flip(node), g);
    if (in_bundle1) { 
        mark_bundle(g, bundle1, bundle_map); 
        update_bundle_nodes(g, *bundle1, updates);
    }
    auto [in_bundle2, bundle2] = find_bundle(node, g);
    if (in_bundle2) { 
        mark_bundle(g, bundle2, bundle_map); 
        update_bundle_nodes(g, *bundle2, updates);
    }
    // TODO: Rationalize if neighbors need to be added if no bundles were found

    /// Current node has been updated
    updates.updated.insert(node);
    updates.updated.insert(g.flip(node)); // TODO: Rationalize if this is necessary
}

handle_t _perform_reduction3(DeletableHandleGraph& g, handle_set_t& orbit, bundle_map_t& bundle_map, node_update_t& updates) {
    /// Unmark the bundle this orbit belongs to
    handle_t o_handle = *orbit.begin();
    bool is_lbundle = bundle_map.count(g.flip(o_handle));

    /// Perform reduction action 3
    handle_t new_handle = reduce_orbit(g, orbit);
    
    /// If the left neighbor was originally a bundle, reinintialize that too
    /// Doing it this way since there's no guarantee a flipping an expired handle will work
    if (is_lbundle) {
        auto [_, lbundle] = find_bundle(g.flip(new_handle), g);
        mark_bundle(g, lbundle, bundle_map);
        update_bundle_nodes(g, *lbundle, updates);
    }
    // TODO: Rationalize if lneighbors need to be added if no bundles were found

    /// Current node has been updated
    updates.updated.insert(new_handle);
    updates.updated.insert(g.flip(new_handle)); // TODO: Rationalize if this is necessary

    return new_handle;
}

void perform_reduction3(DeletableHandleGraph& g, vector<handle_set_t> orbits, bundle_map_t& bundle_map, node_update_t& updates) {
    /// Unmark bundle and balanced if needed
    handle_t o_handle = *(*orbits.begin()).begin();
    Bundle* obundle = bundle_map[o_handle];
    unmark_bundle(g, obundle, bundle_map);

    handle_t new_handle;
    for (auto& orbit : orbits) new_handle = _perform_reduction3(g, orbit, bundle_map, updates);

    /// Reinitialize old bundle
    auto [_, nbundle] = find_bundle(new_handle, g);
    mark_bundle(g, nbundle, bundle_map);
    update_bundle_nodes(g, *nbundle, updates);
}

/// Returns middle node's handle as return_handle
inline bool is_reduction1(const HandleGraph& g, const handle_t& node) {
    return g.get_degree(node, false) == 1 && g.get_degree(node, true) == 1;
}

/// Returns direction of node_id (for the trivial bundle) as return_handle
inline bool is_reduction2(const HandleGraph& g, handle_t& node, bundle_map_t& bundle_map) {
    return bundle_map.count(node) && bundle_map[node]->is_trivial();
}

/// Get neighbors for handle where go_left = false
inline handle_set_t get_neighbors(const HandleGraph& g, const handle_t& handle) {
    handle_set_t neighbors;
    g.follow_edges(handle, false, [&](const handle_t& nei) { neighbors.insert(nei); });
    return neighbors;
}

/// Returns vector of handle_set_t where the handles are pointed into the bundle
vector<handle_set_t> find_orbits(const HandleGraph& g, Bundle& bundle) {
    handle_set_map_t<handle_set_t> nei2orbits;
    /// Left side 
    for (auto& handle : bundle.get_left()) {
        handle_set_t neighbors = get_neighbors(g, g.flip(handle));
        if (!neighbors.size()) continue; /// Skip nodes with no neighbors
        nei2orbits.emplace(neighbors, handle_set_t());
        nei2orbits[neighbors].insert(handle);
    }
    /// Right side
    for (auto& handle : bundle.get_right()) {
        handle_set_t neighbors = get_neighbors(g, handle);
        if (!neighbors.size()) continue; /// Skip nodes with no neighbors
        nei2orbits.emplace(neighbors, handle_set_t());
        nei2orbits[neighbors].insert(g.flip(handle));
    }

    /// Add orbits that have the same size
    vector<handle_set_t> orbits;
    for (auto& [_, orbit_handles] : nei2orbits) {
        if (orbit_handles.size() > 1) {
            orbits.push_back(orbit_handles);
        }
    }
    return orbits;
}

/// Returns true if the final graph is trivial
/// Perform RA 1/3 as many times as possible
/// Perform RA 2 when RA 1/3 actions run out
astree* reduce_graph(DeletableHandleGraph& g) {
    /// Algorithm pseudocode:
    /// Let G be the bidirected graph
    /// Let updated be a set of unchecked nodes
    /// Let <- denote removing some element from the RHS and assigning it to the LHS
    /// Algorithm:
    /// While updated is not empty: 
    ///   If RA2 is possible:
    ///     Perform RA2
    ///   Else if RA3 is possible:
    ///     Perform RA3
    ///   Else if RA1 is possible:
    ///     Perform RA1

    /// Initialize not updated nodes and bundles
    node_update_t updates;
    g.for_each_handle([&] (const handle_t handle) {
        updates.updated.insert(handle);
        updates.updated.insert(g.flip(handle));
    });
    bundle_map_t bundle_map;
    auto bundles = find_bundles(g);
    for (auto& bundle : bundles) mark_bundle(g, bundle, bundle_map);

    /// Main Algorithm
    /// TODO: Add node_update_t as a parameter for all reduction actions
    while (updates.is_action_avail()) {
        vector<handle_set_t> orbits;
        handle_t u = updates.get_updated();
#ifdef DEBUG_REDUCE
        print_node(g, u);
        cout << "[BEGIN] updated.size(): " << (updates.updated.size() + 1) << endl;
#endif /*DEBUG_REDUCE */
        if (!g.has_node(g.get_id(u))) continue; /// Check if node still exists

        /// Check RA2
        if (is_reduction2(g, u, bundle_map)) {
#ifdef DEBUG_REDUCE
            print_node(g, u);
            cout << "\033[35mReduction action 2 available\033[0m" << endl;
#endif /* DEBUG_REDUCE */
            perform_reduction2(g, *bundle_map[u], bundle_map, updates);
        /// Check RA3
//         } else if (bundle_map.count(u) && (orbits = find_orbits(g, *bundle_map[u])).size()) {
// #ifdef DEBUG_REDUCE
//             print_node(g, u);
//             cout << "\033[36mReduction action 3 available\033[0m" << endl;
//             for (size_t i = 0; i < orbits.size(); i++) {
//                 print_node(g, u);
//                 cout << "Orbit " << (i + 1) << ": ";
//                 for (auto& handle : orbits[i]) {
//                     cout << g.get_id(handle) << (g.get_is_reverse(handle) ? "r" : "") << ", ";
//                 }
//                 cout << "\b\b \n";
//             }
// #endif /* DEBUG_REDUCE */
//             perform_reduction3(g, orbits, bundle_map, updates);
//         /// Check RA1
        } else if (is_reduction1(g, u)) {
#ifdef DEBUG_REDUCE
            print_node(g, u);
            cout << "\033[32mReduction action 1 available\033[0m" << endl;
#endif /* DEBUG_REDUCE */
            perform_reduction1(g, u, bundle_map, updates);
        }

#ifdef DEBUG_REDUCE
        print_node(g, u);
        cout << "[END] updated.size(): " << updates.updated.size() << endl;
#endif /*DEBUG_REDUCE */
    }

    return nullptr;
}