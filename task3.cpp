#include <iostream>
#include <climits>
#include <cmath>
using namespace std;

int dp[100];
int bestSplit[100];
int totalMoves;

void hanoi3Peg(int n, char source, char destination, char aux)
{
    if (n == 0)
        return;

    hanoi3Peg(n - 1, source, aux, destination);
    cout << "Move disk " << n << " from Peg " << source << " to Peg " << destination << "\n";
    totalMoves++;
    hanoi3Peg(n - 1, aux, destination, source);
}

void buildDPTable(int maxDisks)\
{
    dp[0] = 0;
    dp[1] = 1;
    bestSplit[1] = 0;

    for (int n = 2; n <= maxDisks; n++)
    {
        dp[n] = INT_MAX;

        for (int k = 1; k < n; k++)
        {
            int cost = 2 * dp[k] + (int)(pow(2, n - k) - 1);

            if (cost < dp[n])
            {
                dp[n]        = cost;
                bestSplit[n] = k;
            }
        }
    }
}

void hanoi4Peg(int n, char source, char destination, char aux1, char aux2)
{
    if (n == 0)
        return;

    if (n == 1)
    {
        cout << "Move disk 1 from Peg " << source << " to Peg " << destination << "\n";
        totalMoves++;
        return;
    }

    int k = bestSplit[n];

    hanoi4Peg(k, source, aux1, destination, aux2);
    hanoi3Peg(n - k, source, destination, aux2);
    hanoi4Peg(k, aux1, destination, source, aux2);
}

void printDPTable(int maxDisks)
{
    cout << "\nDisks | Best k | Min Moves\n";
    cout << "------+--------+----------\n";
    for (int i = 1; i <= maxDisks; i++)
        cout << "  " << i << "   |   " << bestSplit[i] << "    |   " << dp[i] << "\n";
}

int main()
{
    int N = 8;

    buildDPTable(N);

    cout << "--- DP Table (Frame-Stewart) ---\n";
    printDPTable(N);
    cout << "\nMinimum moves for " << N << " disks using 4 pegs: " << dp[N] << "\n";

    cout << "\n--- Divide and Conquer: 3-Peg Hanoi (n = 3) ---\n";
    totalMoves = 0;
    hanoi3Peg(3, 'A', 'C', 'B');
    cout << "Total moves: " << totalMoves << "\n";

    cout << "\n--- Dynamic Programming: 4-Peg Hanoi (n = 4) ---\n";
    totalMoves = 0;
    hanoi4Peg(4, 'A', 'D', 'B', 'C');
    cout << "Total moves: " << totalMoves << "\n";

    cout << "\n--- Full Solution: 4-Peg Hanoi (n = 8) ---\n";
    totalMoves = 0;
    hanoi4Peg(8, 'A', 'D', 'B', 'C');
    cout << "Total moves: " << totalMoves << "\n";

    cout << "\n--- Comparison (n = 8) ---\n";
    cout << "3-Peg D&C  : " << (int)(pow(2, N) - 1) << " moves\n";
    cout << "4-Peg DP   : " << dp[N] << " moves\n";

    return 0;
}