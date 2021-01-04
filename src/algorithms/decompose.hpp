#ifndef VG_ALGORITHMS_DECOMPOSE_HPP_INCLUDED
#define VG_ALGORITHMS_DECOMPOSE_HPP_INCLUDED

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "handle.hpp"
#include "bundle.hpp"
#include "decomposition_tree.hpp"
#include "wang_hash.hpp"
#include "handlegraph/util.hpp"

#define DEBUG_DECOMPOSE
//#define DISABLE_BUILD

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

// Keeps track of edge_t to type
struct edge_t_hash_fn {
    size_t operator()(const edge_t& edge) const {
        return vg::wang_hash_64(as_integer(edge.first)) 
            ^ vg::wang_hash_64(as_integer(edge.second));
    }
};
template<typename T>
using edge_map_t = std::unordered_map<edge_t, T, edge_t_hash_fn>;

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
    DeletableHandleGraph* g = nullptr;

    // Bundle pool creation object
    BundlePool* bpool = nullptr;

    // Keeps track of the largest node id in the graph.
    // Works off the assumption that 1) The graph's ids are nicely compact and
    // 2) The original number of nodes is half of 2 ^ (# of bits of handle_t - 1).
    nid_t nid_counter;

    /// Bookkeeping data structures
    // Node-sides that have been updated and need to be checked.
    node_update_t updates;
    // Maps node-side to corresponding bundle (could not exist if only using
    // balanced bundle).
    bundle_map_t bundle_map;
    // Keeps track of neighbors of a node.
    std::unordered_map<handle_t, size_t> nei_count;
    // Maps node to a decomposition node. If it's a source node then there won't
    // be a value (not a default dict).
    std::unordered_map<nid_t, DecompositionNode*> decomp_map;

    /* Deprecated
    // Keeps track of all the Rule 1 nodes that have been reduced into this edge.
    edge_map_t<std::vector<DecompositionNode*>> freeR1_map;
    // Keeps track of the neighbors of any node (source or derived) that belongs
    // to the original graph. Used to match R1 nodes to their parents. This 
    // structure is only updated during R2 and R3 actions.
    // Handles are oriented such that if traversed in a graph,
    // left_bounds -> id (is_reverse = false) -> right_bounds
    std::unordered_map<nid_t, std::pair<handle_set_t, handle_set_t>> o_neighbors;
    // Keeps track of the left/rightmost boundary nodes of any node (source or 
    // derived) that belonged to the original graph. Used to eliminate nodes
    // when from o_neighbors once a R1 action is performed (since R1 nodes are
    // not considered to be a neighbor of another R1 node).
    std::unordered_map<nid_t, 
        std::pair<std::vector<handle_t>, std::vector<handle_t>>> o_boundaries; 
    */

    /// Bookkeeping functions
    // Initializes base state of bookkeeping data structures.
    void initialize_bookkeeping();
    // Creates a node->bundle mappings for each node in the bundle.
    void mark_bundle(Bundle* bundle);
    // Removes node->bundle mappings for each node in the bundle and recycles
    // Bundle object back to the pool.
    void unmark_bundle(Bundle* bundle);
    // Adds node-sides from bundle to updates.
    inline void update_bundle_nodes(Bundle& bundle);
    // Performs necessary edge renaming if needed.
    inline void rename_edge(edge_t old_edge, edge_t new_edge);

    /// Reduction functions
    // Returns the first neighbor of the given node when traversed with 
    // follow_edges given is_left.
    inline handle_t get_first_neighbor(const handle_t& node, bool is_left);
    // Returns a set of neighbors on the go_left = false side of this node.
    inline handle_set_t get_neighbors(const handle_t& handle);

    // Self-cycle/inversion removal.
    // Removes any self-cycles/inversions present on this node (not dependent
    // on handle direction).
    void remove_self_cycle_inversion(const handle_t& node);

    // Rule 1 
    /** Deprecated: Using stricter versions of rule 1 to retain the tree-like
     * structure.
    // Returns if node-side has a valid rule 1 reduction.
    inline bool is_reduction1(const handle_t& node);
    // Performs rule 1 reduction on the given node (assumes it's valid).
    void perform_reduction1(handle_t& node);
    // Builds decomposition tree node based on the center node and its left and
    // right neighbors.
    void build_reduction1(const handle_t left, const handle_t right, 
            const handle_t center);
    // Attaches a node to its corresponding parents in the decomposition tree.
    void link_r1_node(DecompositionNode* node);
    */

    // Stricter versions of rule 1 that allows retains the tree-like 
    // decomposition structure for a well-behaved graph.
    inline bool is_reduction1_strict(const handle_t& node);
    void perform_reduction1_strict(handle_t& node);
    
    // Rule 2 
    // Returns if node-side has a valid rule 2 reduction.
    inline bool is_reduction2(const handle_t& node);
    // Combines the two nodes in the trivial bundle into one node.
    handle_t reduce_trivial_bundle(Bundle& bundle);
    // Builds decomposition tree node based on the left and right handles.
    void build_reduction2(const nid_t new_nid, const handle_t& left, 
        const handle_t& right);
    // Performs rule 2 reduction on the given trivial bundle (assumes it's valid).
    void perform_reduction2(Bundle& bundle);

    // Rule 3 
    // Returns all orbits with more than one node.
    std::vector<handle_set_t> is_reduction3(Bundle& bundle);
    // Handles in the handle_set_t will be oriented inward such that follow_edges
    // with go_left = false will go to nodes of the other bundleside pointing 
    // in the outward direction (away from the bundle).
    handle_t reduce_orbit(handle_set_t& orbit); 
    // Builds decomposition tree node based on the oribt handles.
    void build_reduction3(const nid_t new_nid, handle_set_t& orbit);
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
    ~DecompositionTreeBuilder();
    // Constructs decomposition tree
    // TODO: Verify that function returns the appropriate object when called 
    // multiple times
    DecompositionNode* construct_tree();
    // Group irreducible nodes given boundary set.
    void group_irreducible(std::unordered_set<nid_t> boundary);
};

#endif /* VG_ALGORITHMS_DECOMPOSE_HPP_INCLUDED */
