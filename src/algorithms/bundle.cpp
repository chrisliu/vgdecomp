#include "bundle.hpp"

/// For each id a pair of bools are stored.
/// The first bool is if the node has been traversed going left
/// The second bool is if the node has been traversed going right
typedef std::unordered_map<nid_t, std::pair<bool, bool>> traversed_t;

std::vector<bundle_t> find_bundles(const HandleGraph* g) {
    std::vector<bundle_t> bundles;
    traversed_t traversed_nodes;

    g->for_each_handle([&] (const handle_t& handle) {
        if (is_unexplored(handle, traversed_nodes, g)) {
            auto ret = is_in_bundle(handle, traversed_nodes, g);
            if (ret.first) {
                bundles.push_back(ret.second);
            }
        }

        if (is_unexplored(handle, traversed_nodes, g, true)) {
            auto ret = is_in_bundle(handle, traversed_nodes, g, true);
            if (ret.first) {
                bundles.push_back(ret.second);
            }
        }
    });

    return bundles;
}

std::pair<bool, bundle_t> is_in_bundle(const handle_t& handle, traversed_t traversed_nodes, const HandleGraph* g, bool is_reverse = false) {
    bundle_t bundle;
    bool handle_left = g->get_is_reverse(handle);
    bool is_right    = (handle_left && is_reverse) || (!handle_left && !is_reverse);
    bundleside_t handle_side   = (is_right) ? bundle.second : bundle.first;
    bundleside_t opposite_side = (is_right) ? bundle.first : bundle.second;

    // Insert edges from opposite side
    g->follow_edges(handle, is_reverse, [&](const handle_t& opp_handle) {
        opposite_side.insert(opp_handle);
    });

    // If there isn't anything, terminate
    if (opposite_side.size() == 0) {
        return std::make_pair(false, bundle);
    }

    // For each node that's on the opposite side of the original handle
    // add it to an unordered set. (Excluded for the first node in the for loop)
    // If a node that's added to the set didn't exist before, terminate
    // the algorithm early and let the function caller know that this handle
    // is not in a bundle
    bool first = true;
    for (const handle_t& opp_handle : opposite_side) {
        bool not_bundle = g->follow_edges(opp_handle, !is_reverse, [&](const handle_t& same_handle) -> bool {
            auto ret = handle_side.insert(same_handle);
            if (!first && ret.second) {
                return false;
            }
            return true;
        });

        if (not_bundle) {
            return std::make_pair(false, bundle);
        }

        if (first) {
            first = false;
        }
    }

    // For each node that's on the opposite side of the nodes on the opposite 
    // side of the original handle add it to an unordered set.
    // If a node that's added to the set didn't exist before, terminate
    // the algorithm early and let the function caller know that this handle
    // is not in a bundle
    for (const handle_t& same_handle : handle_side) {
        bool not_bundle = g->follow_edges(same_handle, is_reverse, [&](const handle_t& opp_handle) -> bool {
            auto ret = opposite_side.insert(opp_handle);
            return ret.second;
        });

        if (not_bundle) {
            return std::make_pair(false, bundle);
        }
    }

    return std::make_pair(true, bundle);
}

inline bool is_unexplored(const handle_t& handle, traversed_t traversed_nodes, const HandleGraph* g, bool is_reverse = false) {
    nid_t id         = g->get_id(handle);
    bool handle_left = g->get_is_reverse(handle);
    bool is_right    = (handle_left && is_reverse) || (!handle_left && !is_reverse); 

    traversed_nodes.emplace(id, std::pair<bool, bool>());
    // Work on this... originally wanted to use the is_right bool to index
    // either the first or second element of the pair
    bool ret;
    if (is_right) {
        ret = traversed_nodes[id].second;
        traversed_nodes[id].second = true;
    } else {
        ret = traversed_nodes[id].first;
        traversed_nodes[id].first = true;
    }
    return ret;
}