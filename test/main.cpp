#include <cstdlib>
#include <iostream>

#include "../src/Edge.hpp"
#include "../src/Node.hpp"

int main() {
	std::cout << "Tests" << std::endl;

	// Check node basic functions
	std::cout << "Testing node objects" << std::endl;
	Node n1 = Node(1);
	Node n2 = Node(1);	// Nodes n1 and n2 should be identical since they have the same id

	Node n3 = Node(2);

	// Check if comparison operator works
	std::cout << (n1 == n2) << "==1" << std::endl;
	std::cout << (n1 == n3) << "==0" << std::endl;
	std::cout << (n2 == n3) << "==0" << std::endl;

	// Check edge functions
	std::cout << "Testing edge object" << std::endl;
	Edge e1 = Edge(&n1, &n2, Edge::DIRECTED_TO);

	std::cout << e1.isChild(&n1) << "==0" << std::endl;
	std::cout << e1.isChild(&n2) << "==1" << std::endl;
	std::cout << e1.getConnectedNode(&n1) << "==" << &n2 << std::endl;
	std::cout << e1.getConnectedNode(&n2) << "==" << &n1 << std::endl;

	// Check node-edge functions
	std::cout << "Testing node-edge functions" << std::endl;
	n1.addEdge(&e1);
	n2.addEdge(&e1);

	std::cout << n1.getOutgoingNodes().size() << "==1" << std::endl;
	std::cout << n1.getIncomingNodes().size() << "==0" << std::endl;
	std::cout << n2.getOutgoingNodes().size() << "==0" << std::endl;
	std::cout << n2.getIncomingNodes().size() << "==1" << std::endl;



	return 0;
}