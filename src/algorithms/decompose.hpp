#ifndef VG_ALGORITHMS_DECOMPOSE_HPP_INCLUDED
#define VG_ALGORITHMS_DECOMPOSE_HPP_INCLUDED

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "handle.hpp"
#include "bundle.hpp"
#include "decomposition_tree.hpp"
#include "wang_hash.hpp"
#include "../../deps/handlegraph/util.hpp"

#define DEBUG_DECOMPOSE

// Declare bookkeeping data structures
// Keeps track of node-side to bundle
using bundle_map_t = std::unordered_map<handle_t, Bundle*>;

// Keeps track of a set of handles to type
using handle_set_t = std::unordered_set<handle_t>;
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
using handle_set_map_t = std::unordered_map<handle_set_t, T, handle_set_t_hash_fn>;

// Keeps track of nodes that need to be checked 
struct node_update_t {
    std::unordered_set<handle_t> updated;

    handle_t get_updated() {
        handle_t first = *updated.begin();
        updated.erase(updated.begin());
        return first;
    }

    bool is_action_avail() {
        return updated.size();
    }
};

/** Decomposition Tree Builder
 * Constructs decomposition tree by reducing a graph.
 */
class DecompositionTreeBuilder {
private:
    // Holds the vg graph that'll be decomposed to find sites
    DeletableHandleGraph* g;

    // Bundle pool creation object
    BundlePool* bpool;

    // Bookkeeping data structures
    node_update_t updates;
    bundle_map_t bundle_map;

    // Bookkepping functions
    // Creates a node->bundle mappings for each node in the bundle.
    void mark_bundle(Bundle* bundle);
    // Removes node->bundle mappings for each node in the bundle and recycles
    // Bundle object back to the pool.
    void unmark_bundle(Bundle* bundle);
    // Adds node-sides from bundle to updates.
    inline void update_bundle_nodes(Bundle& bundle);


    /// Reduction functions
    // Returns the first neighbor of the given node when traversed with 
    // follow_edges given is_left.
    inline handle_t get_first_neighbor(const handle_t& node, bool is_left);
    inline handle_set_t get_neighbors(const handle_t& handle);

    // Rule 1 
    // Returns if node-side has a valid rule 1 reduction.
    inline bool is_reduction1(const handle_t& node);
    // Performs rule 1 reduction on the given node (assumes it's valid).
    void perform_reduction1(handle_t& node);
    
    // Rule 2 
    // Returns if node-side has a valid rule 2 reduction.
    inline bool is_reduction2(const handle_t& node);
    // Combines the two nodes in the trivial bundle into one node.
    handle_t reduce_trivial_bundle(Bundle& bundle);
    // Performs rule 2 reduction on the given trivial bundle (assumes it's valid).
    void perform_reduction2(Bundle& bundle);

    // Rule 3 
    // Returns all orbits with more than one node.
    std::vector<handle_set_t> is_reduction3(Bundle& bundle);
    // Handles in the handle_set_t will be oriented such that follow_edges
    // with go_left = false will go to nodes of the other bundleside.
    handle_t reduce_orbit(handle_set_t& orbit); 
    // Performs rule 3 reduction on the given orbits from a bundle (assumes it's valid).
    void perform_reduction3(std::vector<handle_set_t> orbits);

    // Reduces graph until it's irreducible.
    void reduce();
    // TODO: Group irreducible nodes.
    
    // Debug helpers
#ifdef DEBUG_DECOMPOSE
    void print_node(const handle_t& handle);
#endif /* DEBUG_DECOMPOSE */

public:
    DecompositionTreeBuilder(DeletableHandleGraph* g_);
    // Constructs decomposition tree
    // TODO: Verify that function returns the appropriate object when called 
    // multiple times
    DecompositionNode* construct_tree();
};

#endif /* VG_ALGORITHMS_DECOMPOSE_HPP_INCLUDED */
