#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
using namespace std;

class Graph {
private:
    int n;
    vector<vector<int>> adjMatrix;

public:
    // Constructor
    Graph(int vertices);

    // Input and display
    void inputGraph();
    void displayGraph() const;

    // Getters
    int getWeight(int i, int j) const;
    int getVertices() const;
};

#endif