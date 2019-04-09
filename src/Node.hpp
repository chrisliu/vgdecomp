//
//  Node.hpp
//  
//
//  Created by Chris Liu on 4/9/19.
//

#ifndef Node_hpp
#define Node_hpp

#include "Edge.hpp"

class Edge;

class Node {
	private:
		int id;
		std::vector<Edge*> edges;
		std::vector<Node*> outgoingNodes;
		std::vector<Node*> incomingNodes;
	public:
		Node(int id);
		void addEdge(Edge* edge);
		std::vector<Node*> getOutgoingNodes();
		std::vector<Node*> getIncomingNodes();
		bool operator==(const Node& other);
};

#endif /* Node_hpp */
