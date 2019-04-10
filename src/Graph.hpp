//
//  Graph.hpp
//  
//
//  Created by Chris Liu on 4/9/19.
//

#ifndef Graph_hpp
#define Graph_hpp

#include <vector>
#include "Edge.hpp"
#include "Node.hpp"

class Graph {
	private:
		std::vector<Node*> vertices;
		std::vector<Edge*> edges;
		int getIndex(int id);
		Node* getNode(int id);
		void addEdge(int id1, int id2, int edgeType);
	public:
		void addVertex(int id);
		void addUndirectedEdge(int id1, int id2);
		void addDirectedEdge(int id1, int id2);
		std::vector<Node*> getConnectedVertices(int id);
}

#endif /* Graph_hpp */