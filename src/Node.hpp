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

	// Functions
	public:
		Node(int64_t id);                   // Constructor: must give Node ID
		bool operator==(const Node& other); // Is equal if both Node IDs are equal
}

#endif /* Node_hpp */
