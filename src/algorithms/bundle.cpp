#include "bundle.hpp"
#include <algorithm>
#include <handlegraph/util.hpp>

using namespace std;
using namespace handlegraph;

BundlePool* BundlePool::instance = nullptr;

/// Counter structure
struct Count {
    struct value_type { template<typename T> value_type(const T&) {} };
    void push_back(const value_type&) { count++; }
    size_t count = 0;
};

bool BundleSide::add_node(const handle_t& handle) {
    return nodes.insert(handle).second;
}

void BundleSide::update(const HandleGraph& g) {
    nodes_flipped.clear();
    for (auto& node : nodes) {
        nodes_flipped.insert(g.flip(node));
    }
}

void BundleSide::reset() {
    nodes.clear();
    nodes_flipped.clear();
}

bool BundleSide::is_reversed(const handle_t& handle) const  {
    return nodes_flipped.find(handle) != nodes_flipped.end();
}

bool BundleSide::is_member(const handle_t& handle) const {
    return nodes.find(handle) != nodes.end() ||
        nodes_flipped.find(handle) != nodes_flipped.end();
}

bool BundleSide::iterate_nodes(const function<bool(const handle_t&)>& iteratee, bool is_reversed) const {
    if (is_reversed) {
        for (auto& handle : nodes_flipped) {
            if (!iteratee(handle)) {
                return false;
            }
        }
    } else {
        for (auto& handle : nodes) {
            if (!iteratee(handle)) {
                return false;
            }
        }
    }
    return true;
}

void Bundle::update_bundlesides(const HandleGraph& g) {
    left.update(g);
    right.update(g);
}

void Bundle::define_properties(const HandleGraph& g) {
    update_bundlesides(g);

    is_bundle_trivial = left.size() == 1 && right.size() == 1;

    is_bundle_cyclic = false;
    for (const auto& left_handle : left) {
        is_bundle_cyclic |= right.is_member(left_handle);
    }
}

bool Bundle::traverse_bundle(const handle_t& handle, const function<bool(const handle_t&)>& iteratee) const {
    if (left.is_member(handle)) {
        return right.iterate_nodes(iteratee, left.is_reversed(handle));
    } else if (right.is_member(handle)) {
        return left.iterate_nodes(iteratee, right.is_reversed(handle));
    }
}

bool Bundle::is_reversed(const handle_t& handle) const {
    return (left.is_member(handle) && left.is_reversed(handle)) ||
        (right.is_member(handle) && !right.is_reversed(handle));
}

