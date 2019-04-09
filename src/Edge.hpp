//
//  Edge.hpp
//  
//
//  Created by Chris Liu on 4/9/19.
//

#ifndef Edge_hpp
#define Edge_hpp

#include <stdio.h>
#include <vector>
#include <algorithm>
#include "Node.hpp"

class Edge {
	public:
		static const int DIRECTED_FROM;
		static const int DIRECTED_TO;
		static const int UNDIRECTED;

    private:
    	int edgeType;
    	std::vector<Node*> nodes;

    public:
    	Edge(Node* n1, Node* n2, int edgeType);
    	~Edge();
    	bool isChild(Node* n);
    	Node* getConnectedNode(Node *n);
}

#endif /* Edge_hpp */
