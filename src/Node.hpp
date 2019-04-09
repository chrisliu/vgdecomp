//
//  Node.hpp
//  
//
//  Created by Chris Liu on 4/9/19.
//

#ifndef Node_hpp
#define Node_hpp

#include <stdio.h>

class Node {
	private:
		int id;
		std::vector<Edges*> edges;
	public:
		Node(int id);
		void addEdge(Edge* edge);
		std::vector getOutgoingNodes();
		std::vector getIncomingNodes();
		operator==(const Node& other);
}

#endif /* Node_hpp */
