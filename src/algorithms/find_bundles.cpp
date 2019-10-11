#include "find_bundles.hpp"

// #define CACHE_TEST

/// For each id a pair of bools are stored.
/// The first bool is if the node has been traversed going left
/// The second bool is if the node has been traversed going right
typedef std::unordered_map<nid_t, std::pair<bool, bool>> traversed_t;

std::vector<Bundle> find_bundles(const HandleGraph* g) {
    std::vector<Bundle> bundles;
    traversed_t traversed_nodes;

    g->for_each_handle([&] (const handle_t& handle) {
        if (cache_node(handle, traversed_nodes, g)) {
            auto ret = is_in_bundle(handle, traversed_nodes, g);
            if (ret.first) {
                bundles.push_back(ret.second);
            }
        }

        if (cache_node(handle, traversed_nodes, g, true)) {
            auto ret = is_in_bundle(handle, traversed_nodes, g, true);
            if (ret.first) {
                bundles.push_back(ret.second);
            }
        }
    });

    return bundles;
}

/// Algorithm:
/// Terminology:
///   Same side: Nodes on the same side of starting node <handle>
///   Opposite side: Nodes on the opposite side of starting node <handle>
/// Phase 1:
///   Find all nodes on the <(go_left) ? "left" : "right"> side of the 
///   starting node <handle>. We'll call this opposite side.
/// Phase 2:
///   Find all nodes on the <(go_left) ? "right" : "left"> side for each
///   node in the opposite side. Call each set of same side nodes found per 
///   opposite side node SSS1, SSS2, ... , SSSn. If there exists a difference
///   between SSSa, SSSb isn't an empty set, then it's not a balanced bundle 
///   and terminate early. Otherwise, call the nodes found same side.
/// Phase 3:
///   Find all nodes on the <(go_left) ? "left" : "right"> side fo each node
///   in the same side. If any node found doesn't belong to the opposite side,
///   then it's not a balanced bundle and terminate early. Otherwise, return 
///   the discovered balanced bundle.
///
/// Returns:
///   A pair of (bool, Bundle). The bool indicates if a bundle has been found,
///   true if there's one else if otherwise. The Bundle will either be the 
///   discovered bundle or a placeholder.
std::pair<bool, Bundle> is_in_bundle(const handle_t& handle, traversed_t& traversed_nodes,
    const HandleGraph* g, bool go_left = false
) {
    Bundle bundle;
    // Get which side of the bundle the handle is on
    bool handle_dir     = g->get_is_reverse(handle);
    bool is_handle_left = (handle_dir && go_left) || (!handle_dir && !go_left); // Is handle on the left side

    // Phase 1
    // Insert edges from opposite side
#ifdef CACHE_TEST
    cache_node(handle, traversed_nodes, g, go_left);
#endif /* CACHE_TEST */
    g->follow_edges(handle, go_left, [&](const handle_t& opp_handle) {
        // Cache traversed handle here
        bundle.add_init_node(handle, !is_handle_left);
    });

    // If there isn't anything, terminate
    if (bundle.get_bundleside_size(!is_handle_left) == 0) {
        return std::make_pair(false, bundle);
    }

    // Phase 2
    bool is_first = true;
    bool is_opp_side_consistent = bundle.get_bundleside(!is_handle_left).traverse_bundle(
        [&](const handle_t& opp_handle) {
#ifdef CACHE_TEST
            cache_node(opp_handle, traversed_nodes, g, !go_left);
#endif /* CACHE_TEST */
            if (is_first) {
                g->follow_edges(opp_handle, !go_left, 
                    [&](const handle_t& same_handle) {
                        bundle.add_init_node(same_handle, is_handle_left);
                    }
                );
            } else {
                bool handle_not_added = g->follow_edges(opp_handle, !go_left,
                    [&](const handle_t& same_handle) {
                        return bundle.add_node(same_handle, is_handle_left);
                    }
                );
                
                // Early termination if a handle is added
                if (!handle_not_added) {
                    return false;
                }
            }
            return true;
        }
    );

    if (!is_opp_side_consistent) {
        return std::make_pair(false, bundle);
    }

    // Phase 3
    bool is_same_side_consistent = bundle.get_bundleside(is_handle_left).traverse_bundle(
        [&](const handle_t& same_handle) {
            // Optimization? Don't traverse starting node again.
            if (same_handle != handle) {
#ifdef CACHE_TEST
                cache_node(same_handle, traversed_nodes, g, go_left);
#endif /* CACHE_TEST */
                bool handle_not_added = g->follow_edges(same_handle, go_left, 
                    [&](const handle_t& opp_handle) {
                        return bundle.add_node(opp_handle, !is_handle_left);
                    }
                );

                if (!handle_not_added) {
                    return false;
                }
            }
            return true;
        }
    );

    if (!is_same_side_consistent) {
        return std::make_pair(false, bundle);
    }

    return std::make_pair(true, bundle);
}

inline bool cache_node(const handle_t& handle, traversed_t& traversed_nodes,
    const HandleGraph* g, bool go_left = false
) {
    nid_t id          = g->get_id(handle);
    bool is_reverse   = g->get_is_reverse(handle);
    bool is_left_side = (go_left && !is_reverse) || (!go_left && is_reverse); // On the left side of the node

    traversed_nodes.emplace(id, std::pair<bool, bool>());

    bool ret;
    if (is_left_side) {
        ret = traversed_nodes[id].first;
        traversed_nodes[id].first = true;
    } else {
        ret = traversed_nodes[id].second;
        traversed_nodes[id].second = true;
    }
    return ret;
}