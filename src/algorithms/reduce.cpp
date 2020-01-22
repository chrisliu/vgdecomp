#include "reduce.hpp"
#include <vector>

using namespace handlegraph;
using namespace std;

/// Reduction action 1
inline void reduce_degree_one_node(DeletableHandleGraph& g, handle_t& node) {
    g.destroy_handle(node);
}

/// Reduction action 2
handle_t reduce_trivial_bundle(DeletableHandleGraph& g, Bundle& bundle) {
    handle_t l_handle = *bundle.get_left().begin();
    handle_t r_handle = *bundle.get_right().begin();

    /// Create new node
    handle_t new_handle = g.create_handle("");
    handle_t new_handle_flipped = g.flip(new_handle);

    /// Remap left side edges of lhandle to new node
    g.follow_edges(g.flip(l_handle), false, [&](const handle_t& child_handle) {
        g.create_edge(new_handle_flipped, child_handle);
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
void reduce_bundle(DeletableHandleGraph& g, Bundle& bundle) {
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
}