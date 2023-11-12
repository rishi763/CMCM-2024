import json
import requests
import matplotlib.pyplot as plt
import numpy as np    

with open('simTrial0.json', 'r') as file:
    data = json.load(file, strict=False)
    nr = data['x']
    nc = data['y']
    weights_str = data['weightMap']

    # Convert the comma-separated string to a 2D numpy array
    weights_array = np.fromstring(weights_str, sep=',').reshape(nr, nc)
    plt.axes().set_aspect(2)
    plt.xlim(0, nc + 1)
    plt.ylim(0, nr + 1)
    plt.axis('off')  # Set the aspect ratio outside the loop

    for row in range(1, nr + 1):
        for col in range(1, nc + 1):
            # Use the weight value as the color intensity
            weight_value = weights_array[row - 1, col - 1]/10
            color = plt.cm.RdYlGn(weight_value)
            plt.plot(col, row, marker='s', markersize=4.5, color=color)
    
    plt.axis('off')
    plt.savefig('image.png')
