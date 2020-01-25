#include "reduce.hpp"
#include <vector>

using namespace handlegraph;
using namespace std;

/// Reduction action 1
void reduce_degree_one_node(DeletableHandleGraph& g, handle_t& node) {
    handle_t left_neighbor;
    g.follow_edges(node, true, [&](const handle_t& handle) { left_neighbor = handle; });
    handle_t right_neighbor;
    g.follow_edges(node, false, [&](const handle_t& handle) { right_neighbor = handle; });
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

    Bundle* reduced_bundle = new Bundle();
    (*reduced_bundle).get_left().add_init_node(newl_handle);
    (*reduced_bundle).get_right().add_init_node(newr_handle);
    return reduced_bundle;
}