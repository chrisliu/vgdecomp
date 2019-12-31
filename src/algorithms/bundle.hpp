#ifndef VG_BUNDLE_HPP_INCLUDED
#define VG_BUNDLE_HPP_INCLUDED

#include <utility>
#include <unordered_set>
#include <vector>

#include "../../deps/handlegraph/handle_graph.hpp"
#include "../../deps/handlegraph/iteratee.hpp"
#include "../../deps/handlegraph/types.hpp"

typedef std::unordered_set<handlegraph::handle_t> bundle_set_t;
typedef std::vector<handlegraph::handle_t> bundle_vector_t;

enum Adjacency {
    None,
    Weak,
    Strong
};

class BundleSide {
    private:
        bundle_set_t    bundle_set;
        bundle_vector_t bundle_vector;
        bundle_vector_t bundle_vector_reversed;
        bool            is_bundle_freed;

    public:
        /// Adds a node handle to this bundle side. 
        /// If this bundle side hasn't been cached, the new
        /// node handle will be stored in bundle_set. 
        /// A bool will be returned to indicate whether or
        /// not the node handle already exists.
        /// If the bundle side has been cached, true will
        /// be returned since the nodes in the unordered
        /// set would've been freed.
        /// Parameters:
        ///   1. node: the handle for the node to be added
        /// Returns:
        ///   1. true: if the node handle already exists
        ///            or if this bundle side has been
        ///            cached
        ///   2. false: if the node handle doesn't exist
        bool add_node(const handlegraph::handle_t& node);

        /// Cache bundle side so future iterator operations
        /// will be faster. Converts an unordered set into
        /// a vector. 
        /// This medium article discusses the performance
        /// differences between std::vector and std::unorderd_set
        /// https://medium.com/@gx578007/searching-vector-set-and-unordered-set-6649d1aa7752
        /// Unordered sets are used in the add node process 
        /// for improved adding performance
        /// Vectors are used in the iterating process since 
        /// it's more memory efficient and faster.
        /// bundle_set memory will be freed when this function
        /// is called to conserve memory space.
        void cache(handlegraph::HandleGraph& g);

        /// Iterate through the bundle side with an iterator
        /// Parameters:
        ///   1. iteratee: lambda function that takes a
        ///                const handle_t& as its input
        /// Returns:
        ///   1. true: if the bundle side's nodes have been
        ///            traversed successfully without an
        ///            an early exit triggered by the lambda 
        ///            function
        ///   2. false: if the lambda function triggers an
        ///             early exit
        /// *Note*
        /// Originally I wanted to return a generic iterator
        /// for either bundle_set or bundle_vector depending
        /// on whether or not it's been cached already. However,
        /// I couldn't figure out if it's possible in C++
        void traverse_bundle(const std::function<void(const handlegraph::handle_t&)>& iteratee);

        /// See: https://lemire.me/blog/2017/01/27/how-expensive-are-the-union-and-intersection-of-two-unordered_set-in-c/
        Adjacency get_adjacency(const BundleSide& other) const;

        int size();
};

// *Note*
// Bundle will not check if a node that's added to one side
// already exists on the other side.
// This has not been implemented yet and will need to be to
// double check self cycles. Self cycles could break the 
// algorithm.
class Bundle {
    private:
        typedef std::pair<BundleSide, BundleSide> bundle_t;
        
        bundle_t internal_bundle;
    
        bool is_bundle_trivial;
        bool has_reversed;     
    public:
        /// Add a node handle to the bundle with the side
        /// indicated by is_left. Returns whether or not
        /// the insertion is a success. If it isn't a success
        /// that mean this structure is not a bundle and all
        /// future attempt to add a node will return the same
        /// value.
        /// Parameters:
        ///   1. node: the handle for the node to be added
        ///   2. is_left: the side of a bundle the node
        ///               is going to be added to. 
        ///               true if it's on the left
        ///               false if it's on the right
        /// Returns:
        ///   1. true: if the node is successfully added to
        ///            the side (not a duplicate)
        ///   2. false: if a node is unsuccessfully added
        ///             to the side (is a duplicate) or
        ///             some previous add action resulted
        ///             in this bundle to be invalid
        bool add_node(const handlegraph::handle_t& node, bool is_left = true);

        /// Add a node handle to the bundle with the side
        /// indiacted by is_left. This operation will not
        /// care about duplicate values and should only
        /// be used to initialize a side a bundle.
        /// Parameters:
        ///   1. node: the handle for the node to be added
        ///   2. is_left: the side of a bundle the node
        ///               is going to be added to.
        ///               true if it's on the left
        ///               false if it's on the right
        void add_init_node(const handlegraph::handle_t& node, bool is_left = true);

        int get_bundleside_size(bool is_left);

        /// Access the Bundleside variables for each side
        /// Left side bundle access
        BundleSide get_bundleside(bool is_left);

        void freeze(handlegraph::HandleGraph& g);

        /// Accessor functions
        bool is_trivial();
        void set_trivial(bool is_bundle_trivial_);

        bool has_reversed_node();
        void set_has_reversed_node(bool has_reversed_);
};
#endif /* VG_BUNDLE_HPP_INCLUDED */