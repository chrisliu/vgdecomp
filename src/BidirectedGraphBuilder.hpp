#ifndef BidirectedGraphBuilder_hpp
#define BidirectedGraphBuilder_hpp

/** Using JsonCpp parser
 *  https://github.com/open-source-parsers/jsoncpp
 */

#include <fstream>
#include <iostream>
#include <string>
#include "json/json.h"
#include "Definitions.hpp"

#define DEBUG
#ifdef DEBUG
class BidirectedGraph {
    public:
        void insertEdge(nid_t id1, nid_t id2, bool from_left, bool to_right);
        void insertVertex(nid_t id);        
};
#endif /* DEBUG */

class BidirectedGraphBuilder {
    BidirectedGraph* buildGraph(std::string filename);
    bool saveGraph(std::string filename, BidirectedGraph graph);
};

#endif /* BidirectedGraphBuilder_hpp */