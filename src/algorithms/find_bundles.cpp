#include "find_bundles.hpp"

#ifdef DEBUG_FIND_BUNDLES
#include <iostream> 
#include <sstream>
#include <string>
using namespace std;

inline string handle_info_str(const HandleGraph* g, const handle_t& handle) {
    stringstream ss;
    ss << g->get_id(handle) << ((g->get_is_reverse(handle)) ? "r" : "");
    return ss.str();
} 

#endif /* DEBUG_FIND_BUNDLES */

/// For each id a pair of bools are stored.
/// The first bool is if the node has been traversed going left
/// The second bool is if the node has been traversed going right
typedef std::unordered_map<nid_t, std::pair<bool, bool>> traversed_t;

// Define functions
std::pair<bool, Bundle> is_in_bundle(const handle_t& handle, traversed_t& traversed_nodes,
    const HandleGraph* g, bool go_left = false
);
inline bool cache_node(const handle_t& handle, traversed_t& traversed_nodes,
    const HandleGraph* g, bool go_left = false
);

std::vector<Bundle> find_bundles(const HandleGraph* g) {
    /// Complexity Analysis
    /// Each node is explored once so O(2V) = O(V)
    /// Each edge is traversed at most 3 times O(3E) = O(E)
    /// Therefore the complexity is O(V + E) or at worst O(V^2)
    std::vector<Bundle> bundles;
    traversed_t traversed_nodes;

    g->for_each_handle([&] (const handle_t& handle) {
        if (!cache_node(handle, traversed_nodes, g)) {
            auto ret = is_in_bundle(handle, traversed_nodes, g);
#ifdef DEBUG_FIND_BUNDLES
            cout << g->get_id(handle) << " +: " << ((ret.first) ? "true" : "false") << endl;
#endif /* DEBUG_FIND_BUNDLES */
            if (ret.first) {
                bundles.push_back(ret.second);
            }
        }

        if (!cache_node(handle, traversed_nodes, g, true)) {
            auto ret = is_in_bundle(handle, traversed_nodes, g, true);
#ifdef DEBUG_FIND_BUNDLES
            cout << g->get_id(handle) << " -: " << ((ret.first) ? "true" : "false") << endl;
#endif /* DEBUG_FIND_BUNDLES */
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
    const HandleGraph* g, bool go_left 
) {
    /// Initialization Complexity Analysis
    /// g->get_is_reverse is O(1)
    /// Rest are assignments
    /// Therefore this block is O(1)
    Bundle bundle;
    bool is_not_bundle = false;
    bool has_reversed_node = false;
    // Get which side of the bundle the handle is on
    bool handle_dir     = g->get_is_reverse(handle);
    bool is_handle_left = (handle_dir && go_left) || (!handle_dir && !go_left); // Is handle on the left side

    /// Phase 1 Complexity Analysis
    /// g->follow_edges could be at most O(V-1) = O(V)
    /// Therefore this block is O(V)
    // Insert edges from opposite side
    g->follow_edges(handle, go_left, [&](const handle_t& opp_handle) {
        // Cache traversed handle here
        bundle.add_init_node(opp_handle, !is_handle_left);
        has_reversed_node |= g->get_is_reverse(opp_handle) != handle_dir;
    });

    // If there isn't anything, terminate
    if (bundle.get_bundleside_size(!is_handle_left) == 0) {
        return std::make_pair(false, bundle);
    }

    /// Phase 2 Complexity Analysis
    /// g->follow_edges could be at most O(V)
    /// bundle functions are all O(1) since they are operations on an unordered set
    /// The number of nodes is at most V-1
    /// Therefore the complexity is O(V^2)
    int same_side_count = 0; // Used to verify if there's the same number of same nodes for each opposite node.
    bundle.get_bundleside(!is_handle_left).traverse_bundle(
        [&](const handle_t& opp_handle) {
            cache_node(opp_handle, traversed_nodes, g, !go_left);
            if (!same_side_count) { // same_side_count >= 1 if the first opposite node is traversed since there's at least the starting node.
                g->follow_edges(opp_handle, !go_left, 
                    [&](const handle_t& same_handle) {
                        bundle.add_init_node(same_handle, is_handle_left);
                        has_reversed_node |= g->get_is_reverse(same_handle) != handle_dir;
#ifdef DEBUG_FIND_BUNDLES
                        cout << "(same_init - " << handle_info_str(g, same_handle) << ") is_not_bundle: " << ((is_not_bundle) ? "true" : "false") << endl;
                        cout << "(same_init - " << handle_info_str(g, same_handle) << ") has_reversed_node: " << ((has_reversed_node) ? "true" : "false") << endl;
#endif /* DEBUG_FIND_BUNDLES */
                    }
                );
                same_side_count = bundle.get_bundleside_size(is_handle_left);
#ifdef DEBUG_FIND_BUNDLES
                    cout << "(opp_init - " << handle_info_str(g, opp_handle) << ") is_not_bundle: " << ((is_not_bundle) ? "true" : "false") << endl;
#endif /* DEBUG_FIND_BUNDLES */
            } else {
                int node_count = 0;
                g->follow_edges(opp_handle, !go_left,
                    [&](const handle_t& same_handle) {
                        is_not_bundle |= bundle.add_node(same_handle, is_handle_left);
                        has_reversed_node |= g->get_is_reverse(opp_handle) != handle_dir;
                        node_count++;
#ifdef DEBUG_FIND_BUNDLES
                        cout << "(same - " << handle_info_str(g, same_handle) << ") is_not_bundle: " << ((is_not_bundle) ? "true" : "false") << endl;
#endif /* DEBUG_FIND_BUNDLES */
                    }
                );
                is_not_bundle |= node_count != same_side_count;
#ifdef DEBUG_FIND_BUNDLES
                    cout << "(opp - " << handle_info_str(g, opp_handle) << ") is_not_bundle: " << ((is_not_bundle) ? "true" : "false") << endl;
#endif /* DEBUG_FIND_BUNDLES */
            }
        }
    );
    
#ifdef DEBUG_FIND_BUNDLES
    cout << "(Phase 2) is_not_bundle: " << ((is_not_bundle) ? "true" : "false") << endl;
#endif /* DEBUG_FIND_BUNDLES */

    /// Phase 3 Complexity Analysis
    /// g->follow_edges could be at most O(V)
    /// bundle functions are all O(1) since they are operations on an unordered set
    /// The number of nodes is at most V-1
    /// Therefore the complexity is O(V^2)
    int opposite_side_count = bundle.get_bundleside_size(!is_handle_left); // Used to verify if there's the same number of opposite nodes for each same node.
    bundle.get_bundleside(is_handle_left).traverse_bundle(
        [&](const handle_t& same_handle) {
            if (same_handle != handle) {
                cache_node(same_handle, traversed_nodes, g, go_left);
                int node_count = 0;
                g->follow_edges(same_handle, go_left, 
                    [&](const handle_t& opp_handle) {
                        is_not_bundle |= bundle.add_node(opp_handle, !is_handle_left);
                        has_reversed_node |= g->get_is_reverse(opp_handle) != handle_dir;
                        node_count++;
#ifdef DEBUG_FIND_BUNDLES
                        cout << "(opp - " << handle_info_str(g, opp_handle) << ") is_not_bundle: " << ((is_not_bundle) ? "true" : "false") << endl;
#endif /* DEBUG_FIND_BUNDLES */
                    }
                );
                is_not_bundle |= node_count != opposite_side_count;
#ifdef DEBUG_FIND_BUNDLES
                    cout << "(same - " << handle_info_str(g, same_handle) << ") is_not_bundle: " << ((is_not_bundle) ? "true" : "false") << endl;
#endif /* DEBUG_FIND_BUNDLES */
            }
        }
    );

#ifdef DEBUG_FIND_BUNDLES
    cout << "(Phase 3) is_not_bundle: " << ((is_not_bundle) ? "true" : "false") << endl;
#endif /* DEBUG_FIND_BUNDLES */

    /// Describe Bundle Complexity
    /// All constant assignments
    /// Therefore the complexity is O(1)
    bundle.set_trivial(bundle.get_bundleside_size(true) == 1 && \ 
        bundle.get_bundleside_size(false) == 1);
    bundle.set_has_reversed_node(has_reversed_node);

    return std::make_pair(!is_not_bundle, bundle);
    /// FinalComplexity Analysis
    /// O(V^2)
}

inline bool cache_node(const handle_t& handle, traversed_t& traversed_nodes,
    const HandleGraph* g, bool go_left
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