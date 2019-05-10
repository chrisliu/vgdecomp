import json
from argparse import ArgumentParser, ArgumentTypeError

"""
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

def parse_boolean(bool_input):
    if bool_input.lower() in ('true', 't', 'y', '1'):
        return True
    elif bool_input.lower() in ('false', 'f', 'y', '0'):
        return False
    raise ArgumentTypeError('Boolean value expected')

class GraphGenerator():
    SPLIT_FACTOR    = 0.25  # Percentage of nodes will try to split
    MAX_ULTRABUBBLE = 15    # Maximum size of ultrabubbles
    ACYCLIC         = True  # Acyclic/Cyclic graph
    BUNDLE          = False # Bundles exist/don't exist in graph
    IMPROPER        = False # Enters and exits from the same side

    def __init__(self):
        parser = ArgumentParser(description = 'Creates a easily decomposable graph with the given settings')
        parser.add_argument('nodes', type = int, help = 'number of nodes in the graph')
        parser.add_argument('filename', type = str, help = 'graph export file name')
        parser.add_argument('--split', '--split-factor', type = float, nargs = '?', default = GraphGenerator.SPLIT_FACTOR,
                            help = 'the chance of a node splitting [0, 1]')
        parser.add_argument('--max', '--max-ultrabubble', type = int, nargs = '?', default = GraphGenerator.MAX_ULTRABUBBLE,
                            help = 'maximum length of a single ultrabubble')
        parser.parse_args(namespace=self)

        self.graph = {'test'     : 'hello world',
                      'nodes'    : self.nodes,
                      'split'    : self.split,
                      'max'      : self.max,
                      'filename' : self.filename}

        return
    
    def print_properties(self):
        print("Graph Generator Properties")
        print("Number of nodes:", self.nodes)
        print("Split factor:", self.split)
        print("Max superbubble:", self.max)
        print("Export filename:", self.filename)

    def export(self, readable = True):
        if readable:
            exported = json.dumps(self.graph, indent = 4, sort_keys = True)
        else:
            exported = json.dumps(self.graph)

        with open(self.filename, 'w') as export_file:
            export_file.write(exported)
            export_file.close()

if __name__ == '__main__':
    gGenerator = GraphGenerator()
    gGenerator.print_properties()
    gGenerator.export()
