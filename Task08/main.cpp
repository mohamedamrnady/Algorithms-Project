#include <iostream>
#include <vector>

using namespace std;

  // calculate cut weight
int calculateCut(vector<int> &group) {
        int cut = 0;

       for (int i = 0; i < n; i++) {
           for (int j = i + 1; j < n; j++) {

            if (group[i] != group[j]) {
                cut += weight[i][j];  }
          }
        }
        return cut;
    }
    