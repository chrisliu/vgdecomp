#ifndef Edge_hpp
#define Edge_hpp

#include <vector>
#include <algorithm>
#include "Node.hpp"

using namespace std;
class Node;

class Edge {
	// Variables
	private:
		vector<int64_t> nodes;				// <upstream node id, downstream node id> if it's directed and NOT backwards, it's upstream->downstream
		bool isDirected;					// If this edge is directed or undirected
		bool isBackwards;					// If this edge has been reversed

	// Functions
	public:
		Edge(int64_t id1, int64_t id2, bool isDirected, bool isBackwards = false); // Constructor: id1 is the upstream node, id2 is the downstream node, isDirected states the type of edge, ISN'T backwards by default
		bool reverse();						// Reverses the current direction of the edges and returns whether it's backwards
		int64_t getUpstreamId();			// Gets the upstream node (depends on isBackwards if it's a directed edge)
											// If it's undirected, then it returns the first element in the node vector
		int64_t getDownstreamId();
		int64_t traverse(int64_t id);       // Gives the downstream edge (if available), otherwise "-1" is returned
		bool operator==(const Edge& other); // If directional, edges are equal if their upstream nodes are equal
										    // If undirectional, the upstream edge for other must equal one of the edges in this
};

#endif /* Edge_hpp */
