#include "BidirectedGraphBuilder.hpp"

BidirectedGraph BidirectedGraphBuilder::build_graph(std::string filename) { // No error checks
    std::ifstream graph_file(filename, std::ifstream::binary);

    Json::CharReaderBuilder reader;
    Json::Value graph_json;
    std::string errs;
    bool resp = Json::parseFromStream(reader, graph_file, &graph_json, &errs);
    graph_file.close();

    if (!resp) {
#ifdef DEBUG_GRAPHBUILDER
        std::cout << "(Parse Fail)" << errs << std::endl;
#endif
        return BidirectedGraph();
    }

    if (graph_json["graph"].empty() || 
        !graph_json["graph"]["nodes"].isArray() || !graph_json["graph"]["edges"].isArray()) {
#ifdef DEBUG_GRAPHBUILDER
        std::cout << "Improper input format" << std::endl;
#endif
        return BidirectedGraph();
    }

    BidirectedGraph graph;

    /*
    Json::Value nodes = graph_json["graph"]["nodes"];
    for (int i = 0; i < nodes.size(); i++) {
        graph->add_vertex(nodes[i]["id"].asInt64());
    }
    //*/

    Json::Value edges = graph_json["graph"]["edges"];
    for (int i = 0; i < edges.size(); i++) {
        nid_t id1 = edges[i]["id1"].asInt64();
        nid_t id2 = edges[i]["id2"].asInt64();
        bool from_left = edges[i]["from_left"].asBool();
        bool to_right  = edges[i]["to_left"].asBool();

        graph.add_edge(id1, id2, from_left, to_right);
    }

    return graph;
}

bool BidirectedGraphBuilder::save_graph(std::string filename, BidirectedGraph& graph) {
    /** To be completed when BidirectedGraph is fully complete
     *  Needs access to a list of vertices and edges in graph
     *  See link below to remove duplicate edges
     *  https://stackoverflow.com/questions/1041620/whats-the-most-efficient-way-to-erase-duplicates-and-sort-a-vector
     */
    return true;
}