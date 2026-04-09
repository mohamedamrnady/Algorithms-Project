#include <iostream>
#include "Board.hpp"

// Devise an algorithm for the following task: given a 2^n × 2^n (n > 1) board with one missing square, tile it with
// right trominoes of only three colors so that no pair of trominoes that share an edge have the same color.
// Recall that the right tromino is an L-shaped tile formed by three adjacent squares.
// Use Divide and Conquer and Brute Force technique to solve this problem.
using namespace std;

int main()
{
    // Example run: 2^n x 2^n board with one missing square.
    cout << "Enter n (board size = 2^n x 2^n): ";
    int n;
    cin >> n;
    if (n < 2)
    {
        cout << "Invalid n. Setting to 2.\n";
        n = 2;
    }
    else if (n > 4)
    {
        cout << "Invalid n. Setting to 4.\n";
        n = 4;
    }
    cout << "Enter missing square coordinates 'x y': ";
    int missing_x, missing_y;
    cin >> missing_x >> missing_y;

    cout << endl;

    Board board(n, missing_x, missing_y);
    board.initializeBoardWithTrominos();
    board.printColors();

    return 0;
}
