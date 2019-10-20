#ifndef BidirectedGraphBuilder_hpp
#define BidirectedGraphBuilder_hpp

/** Using JsonCpp parser
 *  https://github.com/open-source-parsers/jsoncpp
 */

#define DEBUG_GRAPHBUILDER

#include <fstream>
#include <iostream>
#include <string>
#include "json/json/json.h"
#include "handlegraph/types.hpp"
#include "BidirectedGraph.hpp"

using namespace handlegraph;

struct BidirectedGraphBuilder {
    /// Returning temporary object won't create a memory issue since modern
    /// C++ compilers perform return value optimization (so BidirectedGraph will
    /// only be created once). This could be verified by adding a debug print 
    /// statement in the constructor of BidirectedGraph.
    /// See more about return value optimization here:
    /// https://en.wikipedia.org/wiki/Copy_elision#Return_value_optimization
    static BidirectedGraph build_graph(std::string filename);
};

#endif /* BidirectedGraphBuilder_hpp */