#ifndef BidirectedGraphBuilder_hpp
#define BidirectedGraphBuilder_hpp

/** Using JsonCpp parser
 *  https://github.com/open-source-parsers/jsoncpp
 */

#define DEBUG_GRAPHBUILDER

#include <fstream>
#include <iostream>
#include <string>
#include "json/json.h"
#include "Definitions.hpp"

#ifndef DEBUG_GRAPHBUILDER
#include "BidirectedGraph.hpp"
#else
class BidirectedGraph {
    public:
        void insertEdge(nid_t id1, nid_t id2, bool from_left, bool to_right) {
            std::cout << "Inserted Edge " << id1 << ((from_left) ? ".left" : ".right") << " <-> " <<
                         id2 << ((to_right) ? ".right" : ".left") << std::endl;
        }
        void insertVertex(nid_t id) {
            std::cout << "Inserted Vertex " << id << std::endl;
        }    
};
#endif /* DEBUG */

class BidirectedGraphBuilder {
    public:
        BidirectedGraph* build_graph(std::string filename);
        bool save_graph(std::string filename, BidirectedGraph graph);
};

#endif /* BidirectedGraphBuilder_hpp */