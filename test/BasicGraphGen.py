import json
"""
Manually generate graphs with given commands
Simplifies bundle toy graph generation

*Note: follows custom json format NOT vg's
"""

def create_empty_graph():
    """
    Generates empty graph dictionary object
    """
    return {'graph':
                {
                 'edges': list(),
                 'nodes': list()
                }
           }

def add_node(graph, nid):
    """
    Adds a node to a graph's list of nodes if
    it's new
    """
    if len(list(filter(lambda node: node['id'] == nid,
                       graph['graph']['nodes']))) == 0:
        graph['graph']['nodes'].append({'id': nid})

def add_edge(graph, id1, id2, from_left = False, to_right = False):
    """
    Adds an edge to a graph
    *Doesn't check if it's new
    """
    
    graph['graph']['edges'].append({'id1': id1,
                                    'id2': id2,
                                    'from_left': from_left,
                                    'to_right': to_right})
    
def create_bundle(graph, lid_s, lid_c, rid_c):
    """
    Creates a bundle from the given parameters
    lid_s: starting node id for the left side
    lid_c: number of nodes on the left side
    rid_c: number of nodes on the right side

    The right side starting id will just continue from the left side
    rid_s = lid_s + lid_c
    """
    for lid in range(lid_s, lid_s + lid_c):
        add_node(graph, lid)
        for rid in range(lid_s + lid_c, lid_s + lid_c + rid_c):
            add_edge(graph, lid, rid)
            add_node(graph, rid)

def export_graph(graph, filename):
    """
    Exports graph into pretty JSON format
    with the given filename
    filename: name of file without extension
    """
    with open("{}.json".format(filename), 'w') as out:
        out.write(json.dumps(graph, indent = 4))

"""
Sample code
graph = create_empty_graph()
create_bundle(graph, 1, 3, 4)
export_graph(graph, 'bundle_only')
"""
