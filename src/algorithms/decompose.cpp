#include "decompose.hpp"
#include <cstdlib>
#include <functional>
#include <unordered_map>
#include <utility>
#include <queue>

#include "find_bundles.hpp"
#include "handlegraph/iteratee.hpp"

#ifdef DEBUG_DECOMPOSE
#include "decomposition_tree.hpp"
#endif /* DEBUG_DECOMPOSE */

/** Node-side representation convention
 * A node-side is represented by a handle such that its neighbors are found
 * via follow_edges with is_left = false.
 */

/** Orbits Terminology 
 * Orbits are borrowed from graph isomorphisms (where an orbit is a set of 
 * vertice could be interchanged with each other in a valid isomorphism).
 * In this case, it's used in bundles where bundle nodes are loosely considered
 * to be in the same orbit if their outward neighbors (neighbors on the side 
 * that's not part of the bundle) are the same. Due to the use of unbalanced
 * bundles, it's possible that nodes classified in the same orbit have different
 * inward neighbors (neighbors from the other bundle side).
 */

#ifdef DEBUG_DECOMPOSE 
#include <assert.h>
#include <iostream> 

void DecompositionTreeBuilder::print_node(const handle_t& handle) {
    std::cout << "(Node " <<  g->get_id(handle) << (g->get_is_reverse(handle) ? "r" : "") << ") ";
}
#endif /* DEBUG_DECOMPOSE */


// Public functions
DecompositionTreeBuilder::DecompositionTreeBuilder(DeletableHandleGraph* g_)
    : g(g_)
    , bpool(BundlePool::get_instance())
{
    // Get the next nid.
    nid_counter = g->max_node_id() + 1;

    // Initialize bookkeeping data structures.
    initialize_bookkeeping();
}

DecompositionTreeBuilder::~DecompositionTreeBuilder() {
    bpool->free();
}

DecompositionNode* DecompositionTreeBuilder::construct_tree() {
    reduce();
    // Only for fully reducible graphs.
#ifndef DISABLE_BUILD
    if (g->get_node_count() == 1) {
        handle_t node;
        g->for_each_handle([&](const handle_t& handle) { node = handle; });
        return decomp_map.at(g->get_id(node));
    }
#endif /* DISABLE_BUILD */
    return nullptr;
}

void DecompositionTreeBuilder::initialize_bookkeeping() {
    // Save the initial state of each node's left and right neighbors.
    g->for_each_handle([&](const handle_t& handle) {
        nid_t nid = g->get_id(handle);

        // Create decomposition source node.
        decomp_map[nid] = new DecompositionNode(nid, Source, 
                g->get_is_reverse(handle));
        
        /* TODO: Deprecated
        // Save left and right neighbors. Ignore any self-cycles/inversions
        // present.
        g->follow_edges(handle, true, [&](const handle_t& nei) {
            if (g->get_id(nei) != nid) o_neighbors[nid].first.insert(nei);
        });
        g->follow_edges(handle, false, [&](const handle_t& nei) {
            if (g->get_id(nei) != nid) o_neighbors[nid].second.insert(nei);
        });

        // Set the boundaries to itself.
        o_boundaries[nid].first.push_back(handle);
        o_boundaries[nid].first.push_back(handle);
        */
    });
}

// Private functions
void DecompositionTreeBuilder::mark_bundle(Bundle* bundle) {
    for (auto& handle : bundle->get_left()) bundle_map[handle] = bundle;
    for (auto& handle : bundle->get_right()) bundle_map[g->flip(handle)] = bundle;
}

void DecompositionTreeBuilder::unmark_bundle(Bundle* bundle) {
    // Go through each bundleside and remove their reference.
    for (auto& handle : bundle->get_left()) bundle_map.erase(handle);
    for (auto& handle : bundle->get_right()) bundle_map.erase(g->flip(handle));

    // Recycle Bundle object
    bpool->return_bundle(bundle);
}

inline void DecompositionTreeBuilder::update_bundle_nodes(Bundle& bundle) {
    for (auto& l_node : bundle.get_left()) updates.updated.insert(l_node);
    for (auto& r_node : bundle.get_right()) updates.updated.insert(g->flip(r_node));
}

inline void DecompositionTreeBuilder::rename_edge(edge_t old_edge, edge_t new_edge) {
    // TODO: Devise more memory efficient method of moving certain values
    // instead of copying.
    
    /* TODO: Deprecated
    // If the current edge has R1 children.
    if (freeR1_map.count(old_edge)) {
        freeR1_map[new_edge].insert(freeR1_map[new_edge].end(),
                freeR1_map[old_edge].begin(), freeR1_map[old_edge].end());
    }
    */
}

inline handle_t DecompositionTreeBuilder::get_first_neighbor(
    const handle_t& node, bool is_left
) {
    handle_t neighbor;
    g->follow_edges(node, is_left, [&](const handle_t& handle) {
        neighbor = handle;
        return false;
    });
    return neighbor;
}

inline handle_set_t DecompositionTreeBuilder::get_neighbors(const handle_t& handle) {
    handle_set_t neighbors;
    g->follow_edges(handle, false, [&](const handle_t& nei) {
        neighbors.insert(nei);
    });
    return neighbors;
}

void DecompositionTreeBuilder::remove_self_cycle_inversion(const handle_t& node) {
    // Self cycle (only one).
    edge_t s_cycle = g->edge_handle(node, node);
    if (g->has_edge(s_cycle)) {
        g->destroy_edge(s_cycle);
        /* TODO: Deprecated
        for (auto node : freeR1_map[s_cycle]) link_r1_node(node);
        */
    }

    // Inversion on the "left".
    edge_t s_inv_l = g->edge_handle(node, g->flip(node));
    if (g->has_edge(s_inv_l)) {
        g->destroy_edge(s_inv_l);
        /* TODO: Deprecated
        for (auto node : freeR1_map[s_inv_l]) link_r1_node(node);
        */
    }

    // Inversion on the "right".
    edge_t s_inv_r = g->edge_handle(g->flip(node), node);
    if (g->has_edge(s_inv_r)) {
        g->destroy_edge(s_inv_r);
        /* TODO: Deprecated
        for (auto node : freeR1_map[s_inv_r]) link_r1_node(node);
        */
    }

}

/* Deprecated
inline bool DecompositionTreeBuilder::is_reduction1(const handle_t& node) {
    return g->get_degree(node, false) == 1 && g->get_degree(node, true) == 1; 
}


void DecompositionTreeBuilder::perform_reduction1(handle_t& node) {
    // Get left and right neighbors of the middle node
    handle_t left_neighbor = get_first_neighbor(node, true);
    handle_t right_neighbor = get_first_neighbor(node, false);

#ifndef DISABLE_BUILD
    // Assign middle node to the corresponding edge.
    build_reduction1(left_neighbor, right_neighbor, node);
#endif // DISABLE_BUILD

    // Remove bundle between left-middle and middle-right
    // If only balanced bundles are used, there MUST be a check for if the
    // bundle exists for each repsective node-side since there's a possiblity
    // that there isn't.
    unmark_bundle(bundle_map[node]);
    // This check must be in place in case the left and right neighbors belong 
    // in the same bundle (due to allowing for unbalanced bundle).
    if (bundle_map.count(g->flip(node))) unmark_bundle(bundle_map[g->flip(node)]);
    
    // Create new edge between the left and right neighbors
    g->create_edge(left_neighbor, right_neighbor);

    // Destroy middle node
    g->destroy_handle(node);

    // Construct a new bundle between the left and right neighbor now that the
    // middle node has been removed. Only the left neighbor needs to checked
    // since the left and right are now connected. Due to the use of unbalanced
    // bundles, all node-sides must belong to a bundle, hence there's no need 
    // to check the first return argument of find_bundle.
    auto [_, bundle] = find_bundle(left_neighbor, *g, false);
    mark_bundle(bundle);

    // Since both the left and right neighbors have been updated, add the 
    // node-side that was adjacent to the middle node.
    updates.updated.insert(left_neighbor);
    updates.updated.insert(g->flip(right_neighbor));
}

void DecompositionTreeBuilder::build_reduction1(const handle_t left,
        const handle_t right, const handle_t center) {
    edge_t edge = g->edge_handle(left, right);
    
    // Get/Create free R1 node for the center node.
    DecompositionNode* free_node = decomp_map[g->get_id(center)]; 
    
    // Make sure to orient such that the direction is from edge.first to
    // edge.second. Will need to reverse if the canonical direction of the edge
    // is not left->right XOR the direction of the center handle is not the 
    // same as the decomp node's handle (if both are true, then 2 flips 
    // is equivalent to no flip).
    if ((g->get_is_reverse(center) != free_node->is_reverse) 
            != (left != edge.first))
        free_node->reverse();

    // Push node to list of free nodes that belong to this edge.
    freeR1_map[edge].push_back(free_node);

    edge_t left_edge = g->edge_handle(left, center);
    for (auto node : freeR1_map[left_edge]) link_r1_node(node);
    edge_t right_edge = g->edge_handle(center, right);
    for (auto node : freeR1_map[right_edge]) link_r1_node(node);

    // Remove this node's boundaries from the o_neighbors of the left and right.
    auto& left_nei = (g->get_is_reverse(left)) ? 
        o_neighbors[g->get_id(left)].first : o_neighbors[g->get_id(left)].second;
    auto& right_nei = (g->get_is_reverse(right)) ? 
        o_neighbors[g->get_id(right)].second : o_neighbors[g->get_id(right)].first;
    auto& [lbound, rbound] = o_boundaries[g->get_id(center)];
    if (g->get_is_reverse(center)) std::swap(lbound, rbound);
    bool lflip = g->get_is_reverse(left) != g->get_is_reverse(center);
    for (auto h : lbound) left_nei.erase((lflip) ? g->flip(h) : h);
    bool rflip = g->get_is_reverse(right) != g->get_is_reverse(center);
    for (auto h : rbound) right_nei.erase((rflip) ? g->flip(h) : h);
}

void DecompositionTreeBuilder::link_r1_node(DecompositionNode* node) {
#ifdef DEBUG_DECOMPOSE
    std::cout << "Linking node " << node->nid << std::endl;
#endif // DEBUG_DECOMPOSE

    // Get nodes that were originally the L/R neighbors of the current
    // decomposition node.
    auto& [left, right] = o_neighbors[node->nid];

    // Flip neighbors if needed.
    if (node->is_reverse) std::swap(left, right);

    // Bind left and right parents along with the correct orientations.
    // TODO: Repeat code aaaaackkkkk!!!!!
    for (auto handle: left) {
        DecompositionNode* parent = decomp_map[g->get_id(handle)];
        // The orientation flag is true when the direction of the parent 
        // decomposition node is not the same as the direction of its handle.
        bool par_rev = parent->is_reverse != (node->is_reverse ^ g->get_is_reverse(handle)); 
        node->left_parents[parent] = par_rev;
        parent->R1children.push_back(node);
    }
    for (auto handle : right) {
        DecompositionNode* parent = decomp_map[g->get_id(handle)];
        bool par_rev = parent->is_reverse != (node->is_reverse ^ g->get_is_reverse(handle));
        node->right_parents[parent] = par_rev;
        parent->R1children.push_back(node);
    }
}
*/

inline bool DecompositionTreeBuilder::is_reduction1_strict(const handle_t& node) {
    // The node must have one left and one right parent. If not, it isn't 
    // a candidate for a rule 1 reduction.
    if (g->get_degree(node, false) != 1 or g->get_degree(node, true) != 1)
        return false;

    // Additionally, the candidate node's left and right neighbors must share 
    // an edge.
    handle_t left_neighbor = get_first_neighbor(node, true);
    handle_t right_neighbor = get_first_neighbor(node, false);
    return g->has_edge(left_neighbor, right_neighbor);
}

void DecompositionTreeBuilder::perform_reduction1_strict(handle_t& node) {
    // Get left and right neighbors.
    handle_t left_neighbor = get_first_neighbor(node, true);
    handle_t right_neighbor = get_first_neighbor(node, false);

    // Destroy original edge between neighbors.
    g->destroy_edge(left_neighbor, right_neighbor);

    // Create new epsilon node that represents the destroyed edge.
    nid_t nid = nid_counter++;
    handle_t epsilon_node = g->create_handle("", nid); 

    g->create_edge(left_neighbor, epsilon_node);
    g->create_edge(epsilon_node, right_neighbor);
    
    // Update the bundles.
    auto [_l, bl] = find_bundle(epsilon_node, *g, false);
    mark_bundle(bl);
    update_bundle_nodes(*bl);

    auto [_r, br] = find_bundle(g->flip(epsilon_node), *g, false);
    mark_bundle(br);
    update_bundle_nodes(*br);

    // Also create epsilon node in decomposition map.
    decomp_map[nid] = new DecompositionNode(nid, Epsilon,
            g->get_is_reverse(epsilon_node));
}

inline bool DecompositionTreeBuilder::is_reduction2(const handle_t& node) {
    return bundle_map.count(node) && bundle_map[node]->is_trivial();
}

handle_t DecompositionTreeBuilder::reduce_trivial_bundle(Bundle& bundle) {
    // Get handle for the left and right nodes of this trivial bundle.
    handle_t l_handle = *bundle.get_left().begin();
    handle_t r_handle = *bundle.get_right().begin();

    // Create new node.
    handle_t new_node = g->create_handle("", nid_counter++);

    // Check if it's a self-cycle (regular or inversion).
    if (g->get_id(l_handle) == g->get_id(r_handle)) {
#ifdef DEBUG_DECOMPOSE
        std::cout << "\033[31mDeleting self cycle\033[0m" << std::endl;
#endif /* DEBUG_DECOMPOSE */
        
        // If it's an inversion, reconnect the edges of the opposite node-side 
        // to the new node. This removes the inversion edge on the node-side and
        // preserves the other neighbors of the opposite node-side.
        if (l_handle != r_handle) {
            g->follow_edges(l_handle, true, [&](const handle_t& l_nei) {
                g->create_edge(l_nei, new_node);
            });
        }
        
        // Destroy the handle to the node with the self-cycle since it's either
        // been duplicated to the new node. For regular self-cycle it would have
        // no other neighbor nodes.
        g->destroy_handle(l_handle);

        return new_node;
    }

    // For trivial bundles that aren't self cycles.
    // Remap left side edges of l_handle to the new node.
    g->follow_edges(l_handle, true, [&](const handle_t& l_nei) {
        g->create_edge(l_nei, new_node);
    });

    // Remap right side edges of r_handle to new node.
    g->follow_edges(r_handle, false, [&](const handle_t& r_nei) {
        g->create_edge(new_node, r_nei);
    });

#ifndef DISABLE_BUILD
    // Build decomposition tree node from this reduction.
    build_reduction2(g->get_id(new_node), l_handle, r_handle);
#endif /* DISABLE_BUILD */

    // Destroy the two nodes of this trivial bundle.
    g->destroy_handle(l_handle);
    g->destroy_handle(r_handle);

    return new_node;
}

void DecompositionTreeBuilder::build_reduction2(const nid_t new_nid,
    const handle_t& left, const handle_t& right
) {
    /* TODO: Deprecated
    // Create new neighbors from left neighbors of left and right neighbors of
    // right. Also create new boundary from left bound of left and right 
    // bound of right.
    if (g->get_is_reverse(left)) {
        for (auto& h : o_neighbors[g->get_id(left)].second)
            o_neighbors[new_nid].first.insert(g->flip(h));
        for (auto& h : o_boundaries[g->get_id(left)].second)
            o_boundaries[new_nid].first.push_back(g->flip(h));
    } else {
        o_neighbors[new_nid].first = o_neighbors[g->get_id(left)].first;
        o_boundaries[new_nid].first = o_boundaries[g->get_id(left)].first;
    }

    if (g->get_is_reverse(right)) {
        for (auto& h : o_neighbors[g->get_id(right)].first)
            o_neighbors[new_nid].second.insert(g->flip(h));
        for (auto& h : o_boundaries[g->get_id(right)].first)
            o_boundaries[new_nid].second.push_back(g->flip(h));
    } else {
        o_neighbors[new_nid].second = o_neighbors[g->get_id(right)].second;
        o_boundaries[new_nid].second = o_boundaries[g->get_id(right)].second;
    }
    */


    // Rename edges.
    handle_t new_handle = g->get_handle(new_nid);
    g->follow_edges(left, true, [&](const handle_t& l_nei) {
        edge_t old_edge = g->edge_handle(l_nei, left);
        edge_t new_edge = g->edge_handle(l_nei, new_handle);
        rename_edge(old_edge, new_edge);        
    });

    g->follow_edges(right, false, [&](const handle_t& r_nei) {
        edge_t old_edge = g->edge_handle(right, r_nei);
        edge_t new_edge = g->edge_handle(new_handle, r_nei);
        rename_edge(old_edge, new_edge);
    });

    // Retrieve left and right decomposition nodes.
    // If there isn't a mapping in decomp_map, then it must mean this node is 
    // a source node and must be created.
    DecompositionNode* left_node = decomp_map[g->get_id(left)]; 
    DecompositionNode* right_node = decomp_map[g->get_id(right)]; 

    // Reverse any decomposition nodes if necessary.
    if (g->get_is_reverse(left) != left_node->is_reverse) left_node->reverse();
    if (g->get_is_reverse(right) != right_node->is_reverse) right_node->reverse();

    // Construct new chain node and save to decomp_map
    DecompositionNode* chain_node = create_chain_node(new_nid, left_node, right_node);
    decomp_map[new_nid] = chain_node;

    edge_t edge = g->edge_handle(left, right);

    /* TODO: Deprecated
    // Assign all R1 children that belong to this edge. Also recursively assign
    // all R1 children of edges that the current edge has inherited (edges
    // that aren't "the same" as the current edge).
    for (auto& node : freeR1_map[edge]) link_r1_node(node);
    */
}

void DecompositionTreeBuilder::perform_reduction2(Bundle& bundle) {
    // Perform rule 2 reduction.
    handle_t node = reduce_trivial_bundle(bundle);

    // Delete node entries of the trivial bundle in bundle_map.
    handle_t left = *bundle.get_left().begin();
    handle_t right = *bundle.get_right().begin();
    unmark_bundle(&bundle);

    // Delete node entries.
    // Need to check both left and right in case it has no left/right neighbors.
    if (bundle_map.count(left)) unmark_bundle(bundle_map[g->flip(left)]);
    if (bundle_map.count(right)) unmark_bundle(bundle_map[right]);

    // Recompute bundles for the left and right side of the node.
    // Find the bundle on the left node-side.
    auto [_, bundle1] = find_bundle(g->flip(node), *g, false);
    mark_bundle(bundle1);
    update_bundle_nodes(*bundle1);

    // Check if the left node-side's bundle has the right node-side. If it does,
    // then there's no need to recompute the same bundle.
    if (!bundle_map.count(node)){
        auto [_, bundle2] = find_bundle(node, *g, false);
        mark_bundle(bundle2);
        update_bundle_nodes(*bundle2);
    }

    // Add the current node that has been updated.
    updates.updated.insert(node);
    updates.updated.insert(g->flip(node));
}

std::vector<handle_set_t> DecompositionTreeBuilder::is_reduction3(Bundle& bundle) {
    // Node-side neighbors->Set of node-sides with these neighbors map
    handle_set_map_t<handle_set_t> nei2orbits;

    // Left side of bundle
    for (auto& handle : bundle.get_left()) {
        handle_set_t neighbors = get_neighbors(g->flip(handle));
        // Skip node-sides with no neighbors
        if (!neighbors.size()) continue; 
        nei2orbits[neighbors].insert(handle);
    }

    // Right side of bundle
    for (auto& handle : bundle.get_right()) {
        handle_set_t neighbors = get_neighbors(handle);
        // Skip node-sides with no neighbors
        if (!neighbors.size()) continue;
        nei2orbits[neighbors].insert(g->flip(handle));
    }

    // Add orbits that have more than one node-side with common neighbors.
    std::vector<handle_set_t> orbits;
    for (auto& [_, orbit_handles] : nei2orbits) {
        if (orbit_handles.size() > 1) {
            orbits.push_back(orbit_handles);
        }
    }
    return orbits;
}

handle_t DecompositionTreeBuilder::reduce_orbit(handle_set_t& orbit) {
    // Create new handle for retracted node.
    handle_t new_node = g->create_handle("", nid_counter++);

#ifndef DISABLE_BUILD
    // Build decomposition tree node from this reduction.
    build_reduction3(g->get_id(new_node), orbit);
#endif /* DISABLE_BUILD */

    // Attach all left neighbors (since it's an orbit, this means all outward
    // nodes will be the same).
    g->follow_edges(*orbit.begin(), true, [&](const handle_t& l_nei) {
        g->create_edge(l_nei, new_node);
    });

    // Attach all right neighbors (since unbalanced bundles are allowed, 
    // inward nodes may not be the same for each node in the orbit).
    // Also destroy orbit nodes since they've been retracted.
    for (auto& o_handle : orbit) {
        g->follow_edges(o_handle, false, [&](const handle_t& r_nei) {
            g->create_edge(new_node, r_nei);
       });
        g->destroy_handle(o_handle);
    }

    return new_node;
}

void DecompositionTreeBuilder::build_reduction3(const nid_t new_nid,
    handle_set_t& orbit
) {
     DecompositionNode* node = new DecompositionNode(new_nid, Split);

     // Group nodes in the orbit that have the same neighbors on the bundle
     // side. This will allow for a finer grain decomposition that would exist
     // if the opposite side was decomposed first. This operation is only 
     // relevant when unbalanced bundles are allowed.
     // TODO: Rationalize if this step is necessary if post-construction
     // operations are used. My rationale atm is that it won't be needed.
     //handle_set_map_t<std::vector<handle_t>> same_orbits;
     //for (auto& handle : orbit) {
         //auto neighbors = get_neighbors(handle);
         //same_orbits[neighbors].push_back(handle);
     //}
     //for (auto& [_, orbit_handles] : same_orbits) {
         //DecompositionNode* orbit_node;
         //// If the node is "unique".
         //if (orbit_handles.size() == 1) {
             //orbit_node = decomp_map[g->get_id(*orbit_handles.begin())];
         //} else {
             //orbit_node = new DecompositionNode(nid_counter++, Split);
         //}

     //}

     handle_t new_handle = g->get_handle(new_nid);
     for (auto& handle : orbit) {
         // Get the decomp node corresponding to the orbit node's id.
         nid_t node_id = g->get_id(handle);
         DecompositionNode* orbit_node = decomp_map[node_id];
        
         // Reverse if needed.
         if (g->get_is_reverse(handle) != orbit_node->is_reverse)
             orbit_node->reverse();

         // Add to split node
         node->add_child(orbit_node);

         // Reassign neighbors.
         /* TODO: Deprecated
         // TODO: This method of inserting the flipped versions is ugly.
         if (g->get_is_reverse(handle)) {
             for (auto& h : o_neighbors[node_id].second)
                 o_neighbors[new_nid].first.insert(g->flip(h));
             for (auto& h : o_neighbors[node_id].first)
                 o_neighbors[new_nid].second.insert(g->flip(h));
             for (auto& h : o_boundaries[node_id].second)
                 o_boundaries[new_nid].first.push_back(g->flip(h));
             for (auto& h : o_boundaries[node_id].first)
                 o_boundaries[new_nid].second.push_back(g->flip(h));
         } else {
             for (auto& h : o_neighbors[node_id].first)
                 o_neighbors[new_nid].first.insert(h);             
             for (auto& h : o_neighbors[node_id].second)
                 o_neighbors[new_nid].second.insert(h);
             o_boundaries[new_nid].first.insert(o_boundaries[new_nid].first.end(),
                     o_boundaries[node_id].first.begin(), 
                     o_boundaries[node_id].first.end());
             o_boundaries[new_nid].second.insert(o_boundaries[new_nid].second.end(),
                     o_boundaries[node_id].second.begin(),
                     o_boundaries[node_id].second.end());
         }
         */

         // Rename edges.
         g->follow_edges(handle, true, [&](const handle_t& l_nei) {
            edge_t old_edge = g->edge_handle(l_nei, handle);
            edge_t new_edge = g->edge_handle(l_nei, new_handle);
            rename_edge(old_edge, new_edge);
         });


         g->follow_edges(handle, true, [&](const handle_t& r_nei) {
            edge_t old_edge = g->edge_handle(handle, r_nei);
            edge_t new_edge = g->edge_handle(new_handle, r_nei);
            rename_edge(old_edge, new_edge);
         });
     }
    
     // Assign split node to decomp_map.
     decomp_map[new_nid] = node;
}

void DecompositionTreeBuilder::perform_reduction3(std::vector<handle_set_t> orbits) {
    // Unmark the bundle that these orbits belong to
    handle_t o_handle = *(*orbits.begin()).begin();
    Bundle* o_bundle = bundle_map[o_handle];
    unmark_bundle(o_bundle);

    // Retract orbits.
    handle_t new_node;
    for (auto& orbit : orbits) {
        new_node = reduce_orbit(orbit);
        updates.updated.insert(new_node);
        updates.updated.insert(g->flip(new_node));
    }

    // Reinitialize retracted bundle.
    auto [_, new_bundle] = find_bundle(new_node, *g, false);
    mark_bundle(new_bundle);
}

void DecompositionTreeBuilder::group_irreducible(std::unordered_set<nid_t> boundary) {
    // If boundary has no elements, the membership map will be all 0 by default.
    // TODO: Optimizations could be made with the membership and canonical 
    // direction maps. They could just keep track of an index. Boundaries are 
    // maintained by the queue.

    // Initialize update set and membership map.
    std::queue<nid_t> updates; // Update queue.
    std::unordered_map<nid_t, nid_t> membership; // Membership of nodes.
    std::unordered_map<nid_t, bool> canon_dir; // Canonical direction.
    for (nid_t node : boundary) {
        // There's no need to initialize the canonical direction of the member
        // nodes since by default their values is false.
        updates.push(node);
        membership[node] = node;
    }

    // Run algorithm.
    while (updates.size()) {
        // Get first element.
        nid_t n = updates.front(); updates.pop();

        handle_t h = g->get_handle(n, canon_dir[n]);

        // TODO: left and right are basically duplicate code.
        // Check "right" neighbors.
        g->follow_edges(h, false, [&](const handle_t& nei) {
            nid_t nei_id = g->get_id(nei);
            
            // If the neighbor is in the boundary set.
            if (boundary.count(nei_id)) return;

            // If the neighbor has a lower membership id than the current node,
            // then it needs to be updated.
            if (membership[nei_id] < membership[n]) {
                membership[nei_id] = membership[n];
                canon_dir[nei_id] = g->get_is_reverse(nei); 
                updates.push(nei_id);
            }
        });

        // Check "left" neighbors.
        g->follow_edges(h, true, [&](const handle_t& nei) {
            nid_t nei_id = g->get_id(nei);
            
            // If the neighbor is in the boundary set.
            if (boundary.count(nei_id)) return;

            // If the neighbor has a lower membership id than the current node,
            // then it needs to be updated.
            if (membership[nei_id] < membership[n]) {
                membership[nei_id] = membership[n];
                canon_dir[nei_id] = g->get_is_reverse(nei);
                updates.push(nei_id);
            }
        });
    }
    
#ifdef DEBUG_DECOMPOSE    
    for (auto& [nid, mid] : membership) {
        std::cout << "Node " << nid << " is a member of " << mid << ". ";
        std::cout << "It's canonical direction is pointing to the ";
        std::cout << (canon_dir[nid] ? "left" : "right");
        std::cout << std::endl;
    }    
#endif /* DEBUG_DECOMPOSE */


    // Group nodes together.
    // Reverse map such that it's (member id, list of nodes).
    std::unordered_map<nid_t, std::vector<nid_t>> reverse_membership;
    for (auto& [nid, mid] : membership) {
        if (nid != mid) reverse_membership[mid].push_back(nid);
    }

    for (auto& [_, members] : reverse_membership) {
        handle_t new_node = g->create_handle("", nid_counter++);
        for (auto& nid : members) {
            handle_t node = g->get_handle(nid, canon_dir[nid]);

            // Copy all edges that connect with a boundary node.
            g->follow_edges(node, false, [&](const handle_t& nei) {
                // Don't create an edge if it connects with another 
                // "member" node.
                if (boundary.count(g->get_id(nei))) return;
                g->create_edge(new_node, nei);
            });
            g->follow_edges(node, true, [&](const handle_t& nei) {
                if (boundary.count(g->get_id(nei))) return;
                g->create_edge(nei, new_node);
            });
            
            // Delete this node.
            g->destroy_handle(node);
        }
    }
}

void DecompositionTreeBuilder::reduce() {
    // Initialize node-sides that need to be checked. 
    updates.updated.clear();
    g->for_each_handle([&](const handle_t& handle) {
        updates.updated.insert(handle);
        updates.updated.insert(g->flip(handle));
    });

    // Initialize bundles that exist in the graph
    bundle_map.clear();
    auto bundles = find_bundles(*g, false);
    for (auto& bundle : bundles) mark_bundle(bundle);

    // Main algorithm
    while(updates.is_action_avail()) {
        std::vector<handle_set_t> orbits;
        handle_t u = updates.get_updated();

#ifdef DEBUG_DECOMPOSE
        print_node(u);
        std::cout << "[BEGIN] updated.size(): " << (updates.updated.size() + 1) << std::endl;
#endif /* DEBUG_DECOMPOSE */

        // Check if node still exists
        if (!g->has_node(g->get_id(u))) continue; 
        remove_self_cycle_inversion(u);

        // Check Rule 2
        if (is_reduction2(u)) {
#ifdef DEBUG_DECOMPOSE
            print_node(u);
            std::cout << "\033[35mReduction action 2 available\033[0m" << std::endl;
#endif /* DEBUG_DECOMPOSE */
            perform_reduction2(*bundle_map[u]);
        // Check Rule 3
        } else if (bundle_map.count(u) && (orbits = is_reduction3(*bundle_map[u])).size()) {
#ifdef DEBUG_DECOMPOSE
            print_node(u);
            std::cout << "\033[36mReduction action 3 available\033[0m" << std::endl;
            for (size_t i = 0; i < orbits.size(); i++) {
                print_node(u);
                std::cout << "Orbit " << (i + 1) << ": ";
                for (const auto& handle : orbits[i]) {
                    std::cout << g->get_id(handle) << (g->get_is_reverse(handle) ? "r" : "") << ", ";
                }
                std::cout << "\b\b \n";
            }
#endif /* DEBUG_DECOMPOSE */
            perform_reduction3(orbits);
        // Check Rule 1
        // Also has the condition that its opposite node side is not in the
        // updated nodes list. This is due to the fact that it's possible to 
        // have a reduction 1 available from one node-side but a reduction 2 
        // possible from the opposite node side. The reduction 1 will stil be
        // found if searched from the opposite node-side.
        // N1 ------------------- N4     N2L has a R1 N2R has a R2
        //    \--- N2 --- N3 ---/        Precedence of R2 > R1
        } else if(!updates.updated.count(g->flip(u)) && is_reduction1_strict(u)) {
#ifdef DEBUG_DECOMPOSE
            print_node(u);
            std::cout << "\033[32mReduction action 1 strict available\033[0m" << std::endl;
#endif /* DEBUG_DECOMPOSE */
            perform_reduction1_strict(u);
        }

#ifdef DEBUG_DECOMPOSE
        print_node(u);
        std::cout << "[END] updated.size(): " << updates.updated.size() << std::endl;
#endif /* DEBUG_DECOMPOSE */
    }
}

