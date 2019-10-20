#include "bundle.hpp"

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

void BundleSide::traverse_bundle(const std::function<void(const handle_t&)>& iteratee) {    
    if (is_bundle_freed) {
        for (const handle_t& node : bundle_vector) {
            iteratee(node);
        }
    } else {
        for (const handle_t& node : bundle_set) {
            iteratee(node);
        }
    }
}

int BundleSide::size() {
    if (is_bundle_freed) {
        return bundle_vector.size();
    }
    return bundle_set.size();
}

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

int Bundle::get_bundleside_size(bool is_left) {
    return get_bundleside(is_left).size();
}

BundleSide Bundle::get_bundleside(bool is_left) {
    if (is_left) {
        return internal_bundle.first;
    }
    return internal_bundle.second;
}

void Bundle::freeze() {
    internal_bundle.first.cache();
    internal_bundle.second.cache();
}

bool Bundle::is_trivial() {
    return is_bundle_trivial; 
}

void Bundle::set_trivial(bool is_bundle_trivial) {
    this->is_bundle_trivial = is_bundle_trivial;
}

bool Bundle::has_reversed_node() {
    return has_reversed;
}

void Bundle::set_has_reversed_node(bool has_reversed) {
    this->has_reversed = has_reversed;
}