#include "find_balanced_bundles.hpp"
#include <unordered_set>

using namespace handlegraph;

#ifdef DEBUG_FIND_BALANCED_BUNDLES
#include <iostream>
#include <sstream>
#include <string>

std::string node_str(const handle_t& handle, const HandleGraph& g) {
    stringstream ss;
    ss << "Node " << g.get_id(handle);
    if (g.get_is_reverse(handle)) ss << " reversed";
    return ss.str();
}
#endif /* DEBUG_FIND_BALANCED_BUNDLES */

using namespace std;

/// Returns true if the handle has been cached or false if otherwise
inline bool cache(const handle_t& handle, unordered_set<handle_t>& cached) { 
    return !cached.insert(handle).second;
}

inline bool cache(const handle_t& handle, unordered_set<handle_t>& cached,
    const HandleGraph& g
) { 
    return !cached.insert(g.flip(handle)).second;
}

/// Returns a Bundle such that if traversing the left side nodes 
/// such that go_left is false will result in the nodes on the right
/// side.
pair<bool, Bundle> is_in_bundle(const handle_t& handle, const HandleGraph& g,
    unordered_set<handle_t>& cached
) {
#ifdef DEBUG_FIND_BALANCED_BUNDLES
    cout << "### " << node_str(handle, g) << " ###" << endl;
#endif /* DEBUG_FIND_BALANCED_BUNDLES */

    Bundle bundle;
    bool is_not_bundle = false;
    bool has_reversed = false;
    bool handle_dir = g.get_is_reverse(handle);

    /// Phase 1: Find right side nodes
    g.follow_edges(handle, false, [&](const handle_t& rhs_handle) {
        bundle.add_init_node(rhs_handle, false);
        has_reversed = g.get_is_reverse(rhs_handle) != handle_dir;
    });

    if (!bundle.get_bundleside_size(false)) {
        return pair(false, bundle);
    }

#ifdef DEBUG_FIND_BALANCED_BUNDLES
    cout << "[Phase 1] RHS nodes:" << endl;
    int count = 1;
    bundle.get_bundleside(false).traverse_bundle([&](const handle_t rhs_handle) {
        cout << "  " << count << ". " << node_str(rhs_handle, g) << endl;
        count++;
    });
#endif /* DEBUG_FIND_BALANCED_BUNDLES */

    /// Phase 2: Find left side nodes and verify all lhs sets are the same
    bool is_first = true;
    int lhs_node_count = 0;
    bundle.get_bundleside(false).traverse_bundle([&](const handle_t& rhs_handle) {
        cache(rhs_handle, cached, g);
        if (is_first) {
            g.follow_edges(rhs_handle, true, [&](const handle_t& lhs_handle) {
                bundle.add_init_node(lhs_handle, true);
                has_reversed |= g.get_is_reverse(lhs_handle) != handle_dir;
                cache(lhs_handle, cached);
                lhs_node_count++;
            });
            is_first = false;
        } else {
            int node_count = 0;
            g.follow_edges(rhs_handle, true, [&](const handle_t& lhs_handle) {
                is_not_bundle |= bundle.add_node(lhs_handle, true);
                has_reversed |= g.get_is_reverse(lhs_handle) != handle_dir;
                cache(lhs_handle, cached);
                node_count++;
            });
            is_not_bundle |= node_count != lhs_node_count;
        }
    });

#ifdef DEBUG_FIND_BALANCED_BUNDLES
    cout << "[Phase 2] LHS nodes:" << endl;
    count = 1;
    bundle.get_bundleside(true).traverse_bundle([&](const handle_t lhs_handle) {
        cout << "  " << count << ". " << node_str(lhs_handle, g) << endl;
        count++;
    });
    cout << "[Phase 2] Is bundle: " << ((!is_not_bundle) ? "True" : "False") << endl;
#endif /* DEBUG_FIND_BALANCED_BUNDLES */

    /// Phase 3: Find right side nodes and verify all rhs sets are the same
    int rhs_node_count = bundle.get_bundleside_size(false);
    bundle.get_bundleside(true).traverse_bundle([&](const handle_t& lhs_handle) {
        if (lhs_handle != handle) {
            int node_count = 0;
            g.follow_edges(lhs_handle, false, [&](const handle_t& rhs_handle) {
                is_not_bundle |= bundle.add_node(rhs_handle, false);
                has_reversed |= g.get_is_reverse(rhs_handle) != handle_dir;
                cache(rhs_handle, cached, g);
                node_count++;
            });
            is_not_bundle |= node_count != rhs_node_count;
        }
    });

#ifdef DEBUG_FIND_BALANCED_BUNDLES
    cout << "[Phase 3] RHS nodes:" << endl;
    count = 1;
    bundle.get_bundleside(false).traverse_bundle([&](const handle_t rhs_handle) {
        cout << "  " << count << ". " << node_str(rhs_handle, g) << endl;
        count++;
    });
    cout << "[Phase 3] Is bundle: " << ((!is_not_bundle) ? "True" : "False") << endl;
#endif /* DEBUG_FIND_BALANCED_BUNDLES */

    /// Phase Descriptor: Describe bundle characteristics
    bundle.set_trivial(bundle.get_bundleside_size(true) == 1 &&
        bundle.get_bundleside_size(false) == 1);
    bundle.set_has_reversed_node(has_reversed);

    return pair(!is_not_bundle, bundle);
}

pair<bool, Bundle> is_in_bundle(const handle_t& handle, const HandleGraph& g) {
    unordered_set<handle_t> cache;
    return is_in_bundle(handle, g, cache);
}

vector<Bundle> find_balanced_bundles(const HandleGraph& g) {
    vector<Bundle> bundles;
    unordered_set<handle_t> cached;

    g.for_each_handle([&](const handle_t& handle) {
        if (!cache(handle, cached)) {
            auto [r_is_bundle, r_bundle] = is_in_bundle(handle, g, cached);
            if (r_is_bundle) {
                bundles.push_back(r_bundle);
            }
        }

        handle_t reversed = g.flip(handle);
        if (!cache(reversed, cached)) {
            auto [l_is_bundle, l_bundle] = is_in_bundle(reversed, g, cached);
            if (l_is_bundle) {
                bundles.push_back(l_bundle);
            }
        }
    });

    return bundles;
}