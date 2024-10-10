import networkx as nx
import matplotlib.pyplot as plt

def read_graph(file_path):
    G = nx.DiGraph()
    with open(file_path, 'r') as f:
        for line in f:
            source, target = map(int, line.strip().split())
            G.add_edge(source, target)
    return G

def read_path(file_path):
    path = []
    with open(file_path, 'r') as f:
        for line in f:
            source, target = map(int, line.strip().split())
            path.append((source, target))
    return path

def plot_graph_and_path(G, path):
    plt.figure(figsize=(12, 8))
    
    pos = nx.circular_layout(G)
    
    nx.draw(G, pos, with_labels=True, node_color='lightblue', node_size=700, arrows=True)
    
    valid_path_edges = [(u, v) for u, v in path if G.has_edge(u, v)]
    invalid_path_edges = [(u, v) for u, v in path if not G.has_edge(u, v)]
    
    if invalid_path_edges:
        print("Edges in the path are not present in the graph:")
        for edge in invalid_path_edges:
            print(f"  {edge[0]} -> {edge[1]}")
    
    for i, (u, v) in enumerate(valid_path_edges):
        nx.draw_networkx_edges(G, pos, edgelist=[(u, v)], edge_color='r', width=2)
        edge_center = ((pos[u][0] + pos[v][0]) / 2, (pos[u][1] + pos[v][1]) / 2)
        plt.text(edge_center[0], edge_center[1], str(i+1), fontsize=10, 
                 bbox=dict(facecolor='white', edgecolor='none', alpha=0.7))
    
    plt.title("DNA Assembly Graph with Best Path")
    plt.axis('off')
    
    plt.subplots_adjust(left=0, right=1, top=1, bottom=0)
    
    plt.savefig("dna_assembly_graph.png", bbox_inches='tight', pad_inches=0.1)
    plt.show()

G = read_graph("graph_structure.txt")
path = read_path("best_path.txt")
plot_graph_and_path(G, path)

print(f"Graph nodes: {G.nodes()}")
print(f"Graph edges: {G.edges()}")
print(f"Path: {path}")
