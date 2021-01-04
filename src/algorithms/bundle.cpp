#include "bundle.hpp"
#include <algorithm>
#include "handlegraph/util.hpp"

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

adjacency_t _get_adjacency_type(const unordered_set<handle_t> side1, const unordered_set<handle_t> side2) {
    Count c;
    set_intersection(side1.begin(), side1.end(), side2.begin(), side2.end(),
        back_insert_iterator(c), [](const handle_t& h1, const handle_t& h2) {
            return as_integer(h1) < as_integer(h2);
        }
    );

    size_t count = c.count;

    if (side1.size() == count && side2.size() == count) {
        return adjacency_t::Strong;
    } else if (count > 0) {
        return adjacency_t::Weak;
    }
    return adjacency_t::None;
}

adjacency_t BundleSide::get_adjacency_type(const BundleSide& other) const {
    adjacency_t res;
    if ((res = _get_adjacency_type(nodes, other.nodes)) != adjacency_t::None ||
        (res = _get_adjacency_type(nodes, other.nodes_flipped)) != adjacency_t::None ||
        (res = _get_adjacency_type(nodes_flipped, other.nodes)) != adjacency_t::None ||
        (res = _get_adjacency_type(nodes_flipped, other.nodes_flipped)) != adjacency_t::None) {
        return res;
    }
    return adjacency_t::None;
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

adjacency_t Bundle::get_adjacency_type(const Bundle& other) const {
    adjacency_t type;
    if ((type = left.get_adjacency_type(other.left)) != adjacency_t::None ||
        (type = left.get_adjacency_type(other.right)) != adjacency_t::None ||
        (type = right.get_adjacency_type(other.left)) != adjacency_t::None ||
        (type = right.get_adjacency_type(other.right)) != adjacency_t::None);
    return type;
}
