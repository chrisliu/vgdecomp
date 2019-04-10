//
//  Tree.hpp
//  
//
//  Created by Chris Liu on 4/9/19.
//

#ifndef Tree_hpp
#define Tree_hpp

#include <vector>
#include "Graph.hpp"

class Tree : public Graph {
	public:
		bool addChild(int childId, int parentId);
		std::vector<Node*> getAncestors(int id);
		std::vector<Node*> getSiblings(int id);
}

#endif /* Tree_hpp */