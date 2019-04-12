#ifndef NodeTraversal_hpp
#define NodeTraversal_hpp

#include <vector>
#include "Node.hpp"
#include "Edge.hpp"

using namespace std;

class NodeTraversal {
	// Variables
	private:
		Node* node;                                           // Node to be traversed
        bool isBackwards;                                     // Whether or not the edges are reversed

	// Functions
	public:
		NodeTraversal(Node* node, bool isBackwards = false);  // Constructor: must give node being wrapped, ISN'T backwards by default
        vector<Edge*> getTraversedEdges(vector<Edge*> edges); // Returns all edges that would be traversed FROM this node
};

#endif /* NodeTraversal_hpp */
