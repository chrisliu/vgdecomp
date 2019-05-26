import json
from argparse import ArgumentParser, ArgumentTypeError

"""

\\
Arguments
    Required:
        1. Nodes (int)
    Optional:
        1. Split factor (float/int) (how often it splits into branches or creates a chain)
        2. Max ultrabubble size (int)
        3. Acyclic (boolean)
        4. Bundle (boolean)
        5. Improper node traversal (boolean)
"""

"""
TEMP: Edges format
{
    “id1”: 0,
    “id2”: 1,
    “from_left”: false,
    “to_right”: false,
}
"""

def parse_boolean(bool_input):
    if bool_input.lower() in ('true', 't', 'y', '1'):
        return True
    elif bool_input.lower() in ('false', 'f', 'y', '0'):
        return False
    raise ArgumentTypeError('Boolean value expected')

class GraphGenerator():
    SPLIT_FACTOR    = 0.25  # Percentage of nodes that will try to split
    MAX_ULTRABUBBLE = 15    # Maximum length of ultrabubbles
    ACYCLIC         = True  # Acyclic/Cyclic graph
    BUNDLE          = False # Bundles exist/don't exist in graph
    IMPROPER        = False # Enters and exits from the same side

    def __init__(self):
        parser = ArgumentParser(description = 'Creates a easily decomposable graph made of nested sites')
        parser.add_argument('nodes', type = int, help = 'number of nodes in the graph')
        parser.add_argument('filename', type = str, help = 'graph export file name')
        parser.add_argument('--split', '--split-factor', type = float, nargs = '?', default = GraphGenerator.SPLIT_FACTOR,
                            help = 'the chance of a node splitting [0, 1]')
        parser.add_argument('--max', '--max-ultrabubble', type = int, nargs = '?', default = GraphGenerator.MAX_ULTRABUBBLE,
                            help = 'maximum length of a single ultrabubble')
        parser.add_argument('--acyclic', type = parse_boolean, nargs = '?', default = GraphGenerator.ACYCLIC,
                            help = 'acyclic/cyclic graph')
        parser.add_argument('--bundle', type = parse_boolean, nargs = '?', default = GraphGenerator.BUNDLE,
                            help = 'true if bundles are included')
        parser.add_argument('--improper', type = parse_boolean, nargs = '?', default = GraphGenerator.IMPROPER,
                            help = 'improper node traversal allowed')
        parser.parse_args(namespace=self)

        self.__create_empty_graph()

        return
    
    def print_properties(self):
        print("Graph Generator Properties")
        print("Number of nodes:", self.nodes)
        print("Split factor:", self.split)
        print("Max superbubble:", self.max)
        print("Acylic?", self.acyclic)
        print("Bundles exist?", self.bundle)
        print("Improper?", self.improper)
        print("Export filename:", self.filename)

    def export(self, readable = True):
        if readable:
            exported = json.dumps(self.graph, indent = 4, sort_keys = True)
        else:
            exported = json.dumps(self.graph)

        with open(self.filename, 'w') as export_file:
            export_file.write(exported)
            export_file.close()

    def __create_empty_graph(self):
        self.graph                   = dict()
        self.graph['graph']          = dict()
        self.graph['graph']['nodes'] = [{'id': i} for i in range(1, self.nodes + 1)]
        self.graph['graph']['edges'] = list()

class GraphStructure():
    def __init__(self, start_id, end_id):
        self.start_id = start_id
        self.end_id   = end_id

    def set_start_id(self, start_id):
        self.start_id = start_id

    def set_end_id(self, end_id):
        self.end_id = end_id

    def get_start_id(self):
        return self.start_id

    def get_end_id(self):
        return self.end_id

    @abstractmethod
    def export(self):
        """
        Returns nodes, edges in this structure
        """
        pass

class Node(GraphStructure):
    def __init__(self, id):
        super().__init__(id, id)
    
    def export(self):
        return [{'id': self.start_id}], []

class Chain(GraphStructure):
    def __init__(self, ):
        super().__init__(-1, -1)
    
    def export(self):
        """
        Returns nodes, edges
        nodes: the nodes contained in this chain
        edges: the edges within this chain (not attached to anything else)
        """

        if self.start_id == -1:
            return [], []

        nodes = list()
        edges = list()

        

        return nodes, edges


if __name__ == '__main__':
    gGenerator = GraphGenerator()
    gGenerator.print_properties()
    gGenerator.export()
