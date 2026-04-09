#include <iostream>
#include <iomanip>
#include "Graph.h"

Graph::Graph(int vertices) {
    if (vertices <= 1) {
        std::cout << "Error: Number of vertices must be > 1. Setting n = 2.\n";
        n = 2;
    }
    else {
        n = vertices;
    }
    adjMatrix.assign(n, vector<int>(n, 0));
}

void Graph::inputGraph() {
    std::cout << "\n--- Input Weighted Adjacency Matrix ---\n";
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            std::cin >> adjMatrix[i][j];
}

void Graph::displayGraph() const {
    std::cout << "\nAdjacency Matrix:\n";
    for (const auto& row : adjMatrix) {
        for (int val : row)
            std::cout << std::setw(4) << val;
        std::cout << std::endl;
    }
}

int Graph::getWeight(int i, int j) const {
    return adjMatrix[i][j];
}

int Graph::getVertices() const {
    return n;
}