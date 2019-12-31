#include "BidirectedGraphBuilder.hpp"
#include <fstream>
#include <iostream>
#include "../deps/json/json/json.h"
#include "../deps/handlegraph/types.hpp"

BidirectedGraph BidirectedGraphBuilder::build_graph(std::string filename) { // No error checks
    std::ifstream graph_file(filename, std::ifstream::binary);

    Json::CharReaderBuilder reader;
    Json::Value graph_json;
    std::string errs;
    bool resp = Json::parseFromStream(reader, graph_file, &graph_json, &errs);
    graph_file.close();

    if (!resp) {
#ifdef DEBUG_GRAPHBUILDER
        std::cerr << "(Parse Fail)" << errs << std::endl;
#endif
        return BidirectedGraph();
    }

    if (graph_json["graph"].empty() || 
        !graph_json["graph"]["nodes"].isArray() || !graph_json["graph"]["edges"].isArray()) {
#ifdef DEBUG_GRAPHBUILDER
        std::cerr << "Improper input format" << std::endl;
#endif
        return BidirectedGraph();
    }

    BidirectedGraph graph;

    Json::Value edges = graph_json["graph"]["edges"];
    for (int i = 0; i < edges.size(); i++) {
        nid_t id1 = edges[i]["id1"].asInt64();
        nid_t id2 = edges[i]["id2"].asInt64();
        bool from_left = edges[i]["from_left"].asBool();
        bool to_right  = edges[i]["to_right"].asBool();

        graph.add_edge(id1, id2, from_left, to_right);
    }

    return graph;
}