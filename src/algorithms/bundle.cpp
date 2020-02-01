#include "bundle.hpp"
#include <algorithm>
#include "../../deps/handlegraph/util.hpp"

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