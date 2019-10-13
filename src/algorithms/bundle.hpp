#ifndef VG_BUNDLE_HPP_INCLUDED
#define VG_BUNDLE_HPP_INCLUDED

#include <unordered_set>
#include <unordered_map>
#include <utility>

#include "../handlegraph/types.hpp"
#include "../handlegraph/iteratee.hpp"

// Need to double check this include statement 
// Shouldn't be necessary but IDE gives me 
// undefinide type error for hnadle_t
#include "../BidirectedGraph.hpp"

class BundleSide {
    private:
        std::unordered_set<handle_t> bundle_set;
        std::vector<handle_t>        bundle_vector;
        bool                         is_bundle_freed;

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
        bool add_node(const handle_t& node);

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
        void cache();

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
        bool traverse_bundle(const std::function<bool(const handle_t&)>& iteratee);

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
        bool     is_invalid;

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
        bool add_node(const handle_t& node, bool is_left = true);

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
        void add_init_node(const handle_t& node, bool is_left = true);

        int get_bundleside_size(bool is_left);

        /// Access the Bundleside variables for each side
        /// Left side bundle access
        BundleSide get_bundleside(bool is_left);

        void freeze();
};
#endif /* VG_BUNDLE_HPP_INCLUDED */