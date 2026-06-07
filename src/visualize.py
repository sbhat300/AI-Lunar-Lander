# VISUALIZATION CODE WRITTEN BY GEMINI, NO OTHER CODE HAS BEEN WRITTEN BY AI

import sys
import networkx as nx
import matplotlib.pyplot as plt
from matplotlib.lines import Line2D

def visualize_neat_tokens(data_string):
    raw_tokens = data_string.split()
    if not raw_tokens:
        print("No data provided.")
        return

    # BULLETPROOF SANITIZER: Fixes all terminal copy-paste fusions
    tokens = []
    for t in raw_tokens:
        # Fix 1: '0' connections fused with the next bias (e.g. "01.8", "00.5", "0-0.5")
        if len(t) > 1 and t[0] == '0' and t[1] != '.':
            tokens.extend(['0', t[1:]])
            
        # Fix 2: A weight fused with a negative bias (e.g. "1.5-0.2")
        elif '-' in t[1:]:
            idx = t.find('-', 1) # Find the minus sign that isn't the first character
            tokens.extend([t[:idx], t[idx:]])
            
        # Normal, un-fused tokens
        else:
            tokens.append(t)

    total_nodes = int(tokens[0])
    num_inputs = int(tokens[1])
    num_outputs = int(tokens[2])

    G = nx.DiGraph()
    ptr = 3  # Start reading after the header

    for idx in range(total_nodes):
        bias = float(tokens[ptr])
        is_output = int(tokens[ptr+1])
        orig_id = int(tokens[ptr+2])
        num_connections = int(tokens[ptr+3])
        ptr += 4

        # Assign layers
        if idx < num_inputs:
            layer = 0
            color = 'lightblue'
        elif is_output == 1:
            layer = 2
            color = 'lightgreen'
        else:
            layer = 1
            color = 'lightgray'

        label = f"ID: {orig_id}\nB: {bias:.2f}"
        G.add_node(idx, label=label, layer=layer, color=color)

        # Parse outgoing connections sequentially
        for _ in range(num_connections):
            target_idx = int(tokens[ptr])
            weight = float(tokens[ptr+1])
            G.add_edge(idx, target_idx, weight=weight)
            ptr += 2

    # Layout generation
    pos = nx.multipartite_layout(G, subset_key="layer", align='horizontal')
    
    colors = [data['color'] for _, data in G.nodes(data=True)]
    labels = {n: data['label'] for n, data in G.nodes(data=True)}
    edge_colors = ['green' if d['weight'] > 0 else 'red' for u, v, d in G.edges(data=True)]
    edge_weights = [max(0.5, abs(d['weight']) * 2) for u, v, d in G.edges(data=True)]

    # Rendering
    plt.figure(figsize=(10, 8))
    nx.draw(G, pos, 
            node_color=colors, 
            node_size=2500, 
            labels=labels, 
            font_size=9, 
            font_weight='bold', 
            arrows=True,
            arrowsize=15,
            edge_color=edge_colors,
            width=edge_weights)

    # Custom legend
    legend_elements = [
        Line2D([0], [0], marker='o', color='w', markerfacecolor='lightblue', markersize=10, label='Input Node'),
        Line2D([0], [0], marker='o', color='w', markerfacecolor='lightgray', markersize=10, label='Hidden Node'),
        Line2D([0], [0], marker='o', color='w', markerfacecolor='lightgreen', markersize=10, label='Output Node'),
        Line2D([0], [0], color='green', lw=2, label='Positive Weight'),
        Line2D([0], [0], color='red', lw=2, label='Negative Weight')
    ]
    plt.legend(handles=legend_elements, loc='upper left')
    
    plt.title("NEAT Network Topology")
    plt.show()

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python visualize.py \"<network_data>\"")
        sys.exit(1)
        
    visualize_neat_tokens(sys.argv[1])