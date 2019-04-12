#ifndef Node_hpp
#define Node_hpp

#include <vector>
#include "Edge.hpp"
using namespace std;

class Edge;

class Node {
	// Variables
	public:
		int64_t id;						    // Node ID
	private:
		vector<Edge*> connectedEdges;       // Edges connected to the node

	// Functions
	public:
		Node(int64_t id);                   // Constructor: must give Node ID
		void addEdge(Edge* edge);			// Adds an edge to be connected to the node
		bool operator==(const Node& other); // Is equal if both Node IDs are equal
}

#endif /* Node_hpp */
