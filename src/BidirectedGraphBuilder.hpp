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

class BidirectedGraphBuilder {
    public:
        BidirectedGraph build_graph(std::string filename);
        bool save_graph(std::string filename, BidirectedGraph& graph);
};

#endif /* BidirectedGraphBuilder_hpp */