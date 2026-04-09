#include <iostream>
#include <vector>
using namespace std;

void printCoins(vector<char> coins) {
    for (char c : coins) cout << c << " ";
    cout << endl;
}
//
bool solve(vector<char> coins) {
    if (coins.empty()) {
        cout << "All coins removed \n";
        return true;
    }

    for (int i = 0; i < coins.size(); i++) {
        if (coins[i] == 'H') {
            vector<char> temp = coins;

            cout << "Remove coin at position " << i << " -> ";

            temp.erase(temp.begin() + i);

            if (i - 1 >= 0)
                temp[i - 1] = (temp[i - 1] == 'H') ? 'T' : 'H';

            if (i < temp.size())
                temp[i] = (temp[i] == 'H') ? 'T' : 'H';

            printCoins(temp);

            if (solve(temp)) return true;

            cout << "Backtrack from position " << i << endl;
        }
    }

    return false;
}

int main() {
    vector<char> coins = { 'H', 'T', 'H', 'T', 'H' };

    cout << "Initial: ";
    printCoins(coins);
    cout << "Steps:\n";

    if (!solve(coins)) {
        cout << "No solution exists \n";
    }

    return 0;
}