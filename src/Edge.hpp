//
//  Edge.hpp
//  
//
//  Created by Chris Liu on 4/9/19.
//

#ifndef Edge_hpp
#define Edge_hpp

#include <vector>
#include <algorithm>
#include "Node.hpp"

class Node;

class Edge {
	public:
		static const int DIRECTED_FROM; // n1->n2
		static const int DIRECTED_TO;   // n2->n1
		static const int UNDIRECTED;    // n1<->n2

    private:
    	std::vector<Node*> nodes;	    // Nodes at each end of the edge

    public:
    	Edge(Node* n1, Node* n2, int edgeType);
    	~Edge();
    	int edgeType;
    	bool isChild(Node* n);
    	Node* getConnectedNode(Node *n);
};

#endif /* Edge_hpp */
