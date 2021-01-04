#include "find_bundles.hpp"
#include <unordered_set>

using namespace handlegraph;
using namespace std;

#ifdef DEBUG_FIND_BUNDLES
#include <iostream>
#include <sstream>
#include <string>

std::string node_str(const handle_t& handle, const HandleGraph& g) {
    stringstream ss;
    ss << "Node " << g.get_id(handle);
    if (g.get_is_reverse(handle)) ss << " reversed";
    return ss.str();
}
using namespace std;
#endif /* DEBUG_FIND_BUNDLES */

/// Returns true if the handle has been cached or false if otherwise
inline bool cache(const handle_t& handle, unordered_set<handle_t>& cached) { 
    return !cached.insert(handle).second;
}

inline bool cache(const handle_t& handle, unordered_set<handle_t>& cached,
    const HandleGraph& g
) { 
    return !cached.insert(g.flip(handle)).second;
}

/// Returns a Bundle such that if traversing the left side nodes when 
/// go_left = false will result in the nodes on the right side of the bundle. 
// TODO: Rewrite algorithm's pseudocode
pair<bool, Bundle*> is_in_bundle(const handle_t& handle, const HandleGraph& g,
    unordered_set<handle_t>& cached, bool is_balanced
) {
#ifdef DEBUG_FIND_BUNDLES
    cout << "### " << node_str(handle, g) << " ###" << endl;
#endif /* DEBUG_FIND_BUNDLES */

    // Fetch bundle object from object pool
    Bundle* bundle = BundlePool::get_instance()->get_bundle();
    // Characteristics of the bundle
    // Flag to check if the bundle is balanced (complete bipartite).
    // Yohei proved that all node-sides belong in some bundle (bipartite). 
    bool is_not_balanced_bundle = false;
    bool handle_dir = g.get_is_reverse(handle);

    // Phase 1: Find right side nodes
    g.follow_edges(handle, false, [&](const handle_t& rhs_handle) {
        bundle->get_right().add_init_node(rhs_handle);
    });

    // If the node-side has no neighbors.
    if (!bundle->get_right().size()) {
        return pair<bool, Bundle*>(false, bundle);
    }

#ifdef DEBUG_FIND_BUNDLES
    cout << "[Phase 1] RHS nodes:" << endl;
    int count = 1;
    for (const auto& rhs_handle : bundle->get_right()) {
        cout << "  " << count << ". " << node_str(rhs_handle, g) << endl;
        count++;
    }
#endif /* DEBUG_FIND_BUNDLES */

    // Phase 2: Find left side nodes and verify all lhs sets are the same
    // All left node-side handles that aren't part of a balanced bundle set. 
    // Only used if we're looking for all bundles (is_balanced = false).
    unordered_set<handle_t> lhs_new; 
    // Flag for the first node. The first node's neighbors are used to compare
    // against all other bundle-side node's neighbors.
    bool is_first = true;
    int lhs_node_count = 0;
    for (const auto& rhs_handle : bundle->get_right()) {
        // Mark node-side as traversed.
        cache(rhs_handle, cached, g);
        if (is_first) {
            // For each neighbor of the init node, add it to the left 
            // bundle-side.
            g.follow_edges(rhs_handle, true, [&](const handle_t& lhs_handle) {
                bundle->get_left().add_init_node(lhs_handle);
                cache(lhs_handle, cached);
                lhs_node_count++;
            });
            is_first = false;
        } else {
            // Counter to track the number of neighbors of the current 
            // right bundle-side node. If the bundle is balanced the neighbor
            // count of any right node-side node should match the neighbor
            // count of the initial right node-side node.
            int node_count = 0;
            g.follow_edges(rhs_handle, true, [&](const handle_t& lhs_handle) {
                // If a new node-side is inserted, this means that the neighbors
                // of this node is not the same as the init node.
                if (bundle->get_left().add_node(lhs_handle)) {
                    is_not_balanced_bundle = true;
                    lhs_new.insert(lhs_handle);
                }
                cache(lhs_handle, cached);
                node_count++;
            });
            is_not_balanced_bundle |= node_count != lhs_node_count;
        }
    }

#ifdef DEBUG_FIND_BUNDLES
    cout << "[Phase 2] LHS nodes:" << endl;
    count = 1;
    for (const auto& lhs_handle : bundle->get_left()) {
        cout << "  " << count << ". " << node_str(lhs_handle, g) << endl;
        count++;
    }
    cout << "[Phase 2] Is bundle: " << ((!is_not_balanced_bundle) ? "true" : "false") << endl;
#endif /* DEBUG_FIND_BUNDLES */

    // Phase 3: Find right side nodes and verify all rhs sets are the same
    // All right node-side handles that aren't part of a balanced bundle set. 
    // Only used if we're looking for all bundles (is_balanced = false).
    unordered_set<handle_t> rhs_new;
    // The number of nodes that are expected to be on the right node-side.
    // This will be used to verify that the neighbors of any node from the left
    // node-side matches what's currently saved (only for is_balanced = true)
    int rhs_node_count = bundle->get_right().size();
    for (const auto& lhs_handle : bundle->get_left()) {
        if (lhs_handle != handle) {
            // Counter to track the number of neighbors of the current left
            // bundle-side node
            int node_count = 0;
            g.follow_edges(lhs_handle, false, [&](const handle_t& rhs_handle) {
                if (bundle->get_right().add_node(rhs_handle)) {
                    is_not_balanced_bundle = true;
                    rhs_new.insert(rhs_handle);
                }
                cache(rhs_handle, cached, g);
                node_count++;
            });
            is_not_balanced_bundle |= node_count != rhs_node_count;
        }
    }

#ifdef DEBUG_FIND_BUNDLES
    cout << "[Phase 3] RHS nodes:" << endl;
    count = 1;
    
    for (const auto& rhs_handle : bundle->get_right()) {
        cout << "  " << count << ". " << node_str(rhs_handle, g) << endl;
        count++;
    }
    cout << "[Phase 3] Is bundle: " << ((!is_not_balanced_bundle) ? "true" : "false") << endl;

    int iteration = 1;
#endif /* DEBUG_FIND_BUNDLES */

    // Phase 4: If the bundle is not balanced and is_balance = false, continue
    // looking for nodes on both sides.
    if (!is_balanced) {
        while (lhs_new.size() || rhs_new.size()) {
#ifdef DEBUG_FIND_BUNDLES
                cout << "--------- Unbalanced nodeside finding iteration " << iteration++ << " ---------" << endl;
#endif /* DEBUG_FIND_BUNDLES */

            for (const auto& lhs_handle : lhs_new) {
#ifdef DEBUG_FIND_BUNDLES
                cout << "### LHS " << node_str(lhs_handle, g) << " ###" << endl;
#endif /* DEBUG_FIND_BUNDLES */
                g.follow_edges(lhs_handle, false, [&](const handle_t& rhs_handle) {
                    if (bundle->get_right().add_node(rhs_handle)) {
                        rhs_new.insert(rhs_handle);
                        cache(rhs_handle, cached, g);
                    }
                });
            }
            lhs_new.clear();

            for (const auto& rhs_handle : rhs_new) {
#ifdef DEBUG_FIND_BUNDLES
                cout << "### RHS " << node_str(rhs_handle, g) << " ###" << endl;
#endif /* DEBUG_FIND_BUNDLES */
                g.follow_edges(rhs_handle, true, [&](const handle_t& lhs_handle) {
                    if (bundle->get_left().add_node(lhs_handle)) {
                        lhs_new.insert(lhs_handle);
                        cache(lhs_handle, cached);
                    }
                });
            }
            rhs_new.clear();
        }
    }

    // Phase Descriptor: Describe bundle characteristics
    bundle->set_balanced(!is_not_balanced_bundle);
    bundle->define_properties(g);

    // If the bundle found is not balanced and we're looking for balanced
    // bundles.
    if (is_balanced && is_not_balanced_bundle) 
        return pair<bool, Bundle*>(false, nullptr);
    return pair<bool, Bundle*>(true, bundle);
}

pair<bool, Bundle*> find_bundle(const handle_t& handle, const HandleGraph& g,
        bool is_balanced) {
    unordered_set<handle_t> cache;
    auto [is_bundle, bundle] = is_in_bundle(handle, g, cache, is_balanced);
    return pair<bool, Bundle*>(is_bundle, bundle);
}

vector<Bundle*> find_bundles(const HandleGraph& g, bool is_balanced) {
    vector<Bundle*> bundles;
    unordered_set<handle_t> cached;

    g.for_each_handle([&](const handle_t& handle) {
        if (!cache(handle, cached)) {
            auto [r_is_bundle, r_bundle] = is_in_bundle(handle, g, cached,
                    is_balanced);
            if (r_is_bundle) bundles.push_back(r_bundle);
        }

        handle_t reversed = g.flip(handle);
        if (!cache(reversed, cached)) {
            auto [l_is_bundle, l_bundle] = is_in_bundle(reversed, g, cached,
                    is_balanced);
            if (l_is_bundle) bundles.push_back(l_bundle);
        }
    });

    return bundles;
}
