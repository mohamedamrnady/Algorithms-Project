#include "brute_force.hpp"
#include <limits>
using namespace std;

// distance squared between two points
double dist2(const Point& a, const Point& b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return dx * dx + dy * dy;
}

// compute centroid of cluster c
Point getCenter(const vector<Point>& points, const vector<int>& assign, int c) {
    double sx = 0, sy = 0;
    int cnt = 0;

    for (int i = 0; i < (int)points.size(); i++) {
        if (assign[i] == c) {
            sx += points[i].x;
            sy += points[i].y;
            cnt++;
        }
    }

    Point center{0, 0};
    if (cnt > 0) {
        center.x = sx / cnt;
        center.y = sy / cnt;
    }
    return center;
}

// check no empty clusters
bool valid(const vector<int>& assign, int k) {
    vector<int> cnt(k, 0);
    for (int x : assign) cnt[x]++;
    for (int i = 0; i < k; i++)
        if (cnt[i] == 0) return false;
    return true;
}

// calculate cost
double getCost(const vector<Point>& points, const vector<int>& assign, int k) {
    if (!valid(assign, k)) return 1e18;

    vector<Point> centers(k);
    for (int c = 0; c < k; c++)
        centers[c] = getCenter(points, assign, c);

    double cost = 0;
    for (int i = 0; i < (int)points.size(); i++) {
        cost += dist2(points[i], centers[assign[i]]);
    }

    return cost;
}

// recursive brute force
void dfs(const vector<Point>& points, int k, int i,
         vector<int>& cur, ClusterResult& best) {

    if (i == (int)points.size()) {
        double cost = getCost(points, cur, k);
        if (cost < best.cost) {
            best.cost = cost;
            best.assignment = cur;
        }
        return;
    }

    for (int c = 0; c < k; c++) {
        cur[i] = c;
        dfs(points, k, i + 1, cur, best);
    }
}

ClusterResult solveBruteForce(const vector<Point>& points, int k) {
    ClusterResult res;
    res.assignment = vector<int>(points.size(), -1);
    res.cost = numeric_limits<double>::max();

    if (k > (int)points.size()) return res;

    vector<int> cur(points.size(), 0);
    dfs(points, k, 0, cur, res);

    return res;
}