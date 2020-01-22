#ifndef VG_ALGORITHMS_BUNDLE_HPP_INCLUDED
#define VG_ALGORITHMS_BUNDLE_HPP_INCLUDED

#include <unordered_set>
#include <vector>

#include "../../deps/handlegraph/types.hpp"
#include "../../deps/handlegraph/handle_graph.hpp"

enum Adjacency {
    None,
    Weak,
    Strong
};

/// Glorified wrapper for std::unordered_set<handle_t>
class BundleSide {
    private:
        std::unordered_set<handlegraph::handle_t> nodes;
        std::unordered_set<handlegraph::handle_t> nodes_flipped;
    public:
        /// Returns true if successful addition, false if otherwise
        bool add_node(const handlegraph::handle_t& handle);

        void add_init_node(const handlegraph::handle_t& handle) {
            add_node(handle);
        }

        size_t size() const { return nodes.size(); }
        
        std::unordered_set<handlegraph::handle_t>::iterator begin() {
            return nodes.begin();
        }

        std::unordered_set<handlegraph::handle_t>::iterator end() {
            return nodes.end();
        }

        std::unordered_set<handlegraph::handle_t>::const_iterator cbegin() const {
            return nodes.cbegin();
        }

        std::unordered_set<handlegraph::handle_t>::const_iterator cend() const {
            return nodes.cend();
        }

        void update(const handlegraph::HandleGraph& g);

        Adjacency get_adjacency_type(const BundleSide& other) const;

        std::vector<handlegraph::handle_t> get_nodes() { return std::vector<handlegraph::handle_t>(nodes.begin(), nodes.end()); }
};

/// Glorified wrapper for std::pair<BundleSide, BundleSide>
class Bundle {
    private:
        BundleSide left;
        BundleSide right;
        bool is_bundle_trivial;
        bool has_reversed;

    public:
        BundleSide& get_bundleside(bool is_left) {
            return is_left ? left : right;
        }
        
        BundleSide& get_left() {
            return left;
        }

        BundleSide& get_right() {
            return right;
        }

        bool is_trivial() { return is_bundle_trivial; }
        void set_trivial(bool is_bundle_trivial_) { is_bundle_trivial = is_bundle_trivial_;}

        bool has_reversed_node() { return has_reversed; }
        void set_has_reversed_node(bool has_reversed_) { has_reversed = has_reversed_; }

        void update_bundlesides(const handlegraph::HandleGraph& g);
};

#endif /* VG_ALGORITHMS_BUNDLE_HPP_INCLUDED */