#ifndef BRUTE_FORCE_H
#define BRUTE_FORCE_H

#include <vector>
#include "point.hpp"

using namespace std;

struct ClusterResult {
    vector<int> assignment;
    double cost;
};

// brute force clustering
ClusterResult solveBruteForce(const vector<Point>& points, int k);

#endif