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
        std::cout << g->get_id(node) << std::endl;
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
#ifdef DEBUG_DECOMPOSE
        std::cout << "\033[31mDeleting self cycle\033[0m" << std::endl;
#endif /* DEBUG_DECOMPOSE */
        // Remove original bundle. With unbalanced bundles removing left and 
        // right would result in a segfault (due to left and right being in 
        // the same bundle).
        unmark_bundle(bundle_map[node]);
        g->destroy_edge(s_cycle);
        // Recompute bundles.
        auto [_r, bl] = find_bundle(g->flip(node), *g, false);
        mark_bundle(bl);
        auto [_l, br] = find_bundle(node, *g, false);
        mark_bundle(br);

        // Mark self-cycle in decomposition node. 
        decomp_map[g->get_id(node)]->scycle = true;
    }

    // Inversion on the "left".
    edge_t s_inv_l = g->edge_handle(g->flip(node), node);
    if (g->has_edge(s_inv_l)) {
#ifdef DEBUG_DECOMPOSE
        std::cout << "\033[31mDeleting self inversion (L)\033[0m" << std::endl;
#endif /* DEBUG_DECOMPOSE */
        unmark_bundle(bundle_map[g->flip(node)]);
        g->destroy_edge(s_inv_l);
        auto [_r, bl] = find_bundle(g->flip(node), *g, false);
        mark_bundle(bl);

        // Mark self-inversion left in decomposition node.
        DecompositionNode* decomp = decomp_map[g->get_id(node)];
        decomp->sinv[g->get_is_reverse(node) != decomp->is_reverse] = true;
    }

    // Inversion on the "right".
    edge_t s_inv_r = g->edge_handle(node, g->flip(node));
    if (g->has_edge(s_inv_r)) {
#ifdef DEBUG_DECOMPOSE
        std::cout << "\033[31mDeleting self inversion (R)\033[0m" << std::endl;
#endif /* DEBUG_DECOMPOSE */
        std::cout << "SEG" << std::endl;
        g->destroy_edge(s_inv_r);
        auto [_l, br] = find_bundle(node, *g, false);
        mark_bundle(br);

        // Mark self-inversion left in decomposition node.
        DecompositionNode* decomp = decomp_map[g->get_id(node)];
        decomp->sinv[g->get_is_reverse(node) == decomp->is_reverse] = true;
    }

}

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
    // Only unmarking left neighbors since with unbalanced bundles the right
    // neighbor must be in it.
    auto obundle = bundle_map[left_neighbor];
    unmark_bundle(obundle);

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

     for (auto& handle : orbit) {
         // Get the decomp node corresponding to the orbit node's id.
         nid_t node_id = g->get_id(handle);
         DecompositionNode* orbit_node = decomp_map[node_id];
        
         // Reverse if needed.
         if (g->get_is_reverse(handle) != orbit_node->is_reverse)
             orbit_node->reverse();

         // Add to split node
         node->add_child(orbit_node);
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
        if (not g->has_node(g->get_id(u))) continue;

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

