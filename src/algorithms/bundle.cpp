#include "bundle.hpp"

/***********************************************
 * Bundle implementation
 ***********************************************/
bool Bundle::add_node(const handle_t& node, bool is_left) {
    if (is_left) {
        return internal_bundle.first.add_node(node);
    } else {
        return internal_bundle.second.add_node(node);
    }
}

void Bundle::add_init_node(const handle_t& node, bool is_left) {
    add_node(node, is_left);
}

BundleSide Bundle::get_left_side() {
    return internal_bundle.first;
}

BundleSide Bundle::get_right_side() {
    return internal_bundle.second;
}

/***********************************************
 * BundleSide implementation
 ***********************************************/

bool BundleSide::add_node(const handle_t& node) {
    if (is_bundle_freed) {
        return true;
    }

    return bundle_set.insert(node).second;
}

void BundleSide::cache() {
    for (const handle_t& node : bundle_set) {
        bundle_vector.push_back(node);
    }

    // Should prompt C++ to clear the memory
    bundle_set = std::unordered_set<handle_t>();
    // Else this is the method I came up with
    /*
    bundle_set.clear();
    bundle_set.rehash(1);
    */

   is_bundle_freed = true;
}

template <typename Iteratee>
bool BundleSide::traverse_bundle(const Iteratee& iteratee) {
    auto bool_iteratee = BoolReturningWrapper<Iteratee, handle_t>::wrap(iteratee);
    
    auto nodes = (is_bundle_freed) ? bundle_vector : bundle_set;
    for (const handle_t& node : nodes) {
        if (!bool_iteratee(node)) {
            return false;
        }
    }
    return true;
}