#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <vector>

class Board
{
    class Tile
    {
        char color;    // color of the tile, represented by a character
        int trominoId; // id of the tromino covering this square (-1 if none)
    public:
        Tile() : color('W'), trominoId(-1) {}             // default constructor for an empty tile
        Tile(char color) : color(color), trominoId(-1) {} // constructor to initialize the tile color

        char getColor() const
        {
            return color;
        }
        int getTrominoId() const
        {
            return trominoId;
        }

        void setTrominoId(int id)
        {
            trominoId = id;
        }

        void setColor(char new_color)
        {
            color = new_color;
        }
    };
    int width; // board size = 2^n

    // x = 0, y = 0 is the top left corner of the board
    int missing_x; // x coordinate of the missing square
    int missing_y; // y coordinate of the missing square

    Tile **tiles; // 2D array of tiles representing the board

    int nextTrominoId = 0;

    void freeTiles()
    {
        if (!tiles)
            return;
        for (int i = 0; i < width; ++i)
        {
            delete[] tiles[i];
        }
        delete[] tiles;
        tiles = nullptr;
    }

    void placeTromino(int id, int x1, int y1, int x2, int y2, int x3, int y3)
    {
        tiles[x1][y1].setTrominoId(id);
        tiles[x2][y2].setTrominoId(id);
        tiles[x3][y3].setTrominoId(id);
    }

    void tileRec(int origin_x, int origin_y, int width, int miss_x, int miss_y)
    {
        // base case: 2x2 board with one missing square can be covered by a single tromino
        // also serves as the stopping condition for the recursion
        if (width == 2)
        {
            const int id = nextTrominoId++;
            for (int dx = 0; dx < 2; ++dx)
            {
                for (int dy = 0; dy < 2; ++dy)
                {
                    const int x = origin_x + dx;
                    const int y = origin_y + dy;
                    if (x == miss_x && y == miss_y)
                        continue;
                    tiles[x][y].setTrominoId(id);
                }
            }
            return;
        }

        const int half = width / 2;
        const int mid_x = origin_x + half;
        const int mid_y = origin_y + half;

        const bool left = miss_x < mid_x;
        const bool top = miss_y < mid_y;
        const int missingQuadrant = (top ? 0 : 2) + (left ? 0 : 1);

        // Center 2x2 squares (one in each quadrant)
        const int center_points[4][2] = {
            {mid_x - 1, mid_y - 1}, // TL
            {mid_x, mid_y - 1},     // TR
            {mid_x - 1, mid_y},     // BL
            {mid_x, mid_y}          // BR
        };

        // Since each of these squares belongs to a different quadrant,
        // this tromino effectively creates a "missing square" in each of the other three quadrants,
        // allowing us to recurse on all four quadrants with the same logic.
        const int centerId = nextTrominoId++;
        for (int i = 0; i < 4; ++i)
        {
            if (i == missingQuadrant)
                continue;
            const int cx = center_points[i][0];
            const int cy = center_points[i][1];
            tiles[cx][cy].setTrominoId(centerId);
        };

        // Recurse into each quadrant.
        // Quadrants without the original missing square use the corresponding
        // center square as their "missing" square.
        for (int i = 0; i < 4; ++i)
        {
            int cx, cy;
            if (i == missingQuadrant)
            {
                cx = miss_x;
                cy = miss_y;
            }
            else
            {
                cx = center_points[i][0];
                cy = center_points[i][1];
            }
            // const int missingQuadrant = (top ? 0 : 2) + (left ? 0 : 1);

            const int org_x = (i % 2 == 0) ? origin_x : mid_x;
            const int org_y = (i < 2) ? origin_y : mid_y;

            tileRec(org_x, org_y, half, cx, cy);
        }
    }

    static bool backtrackColoring(const std::vector<std::vector<int>> &adj, const std::vector<int> &order,
                                  std::vector<int> &colors, int idx)
    {
        if (idx == static_cast<int>(order.size()))
            return true;

        const int v = order[idx];
        bool forbidden[3] = {false, false, false};
        for (int u : adj[v])
        {
            if (colors[u] != -1)
                forbidden[colors[u]] = true;
        }

        for (int c = 0; c < 3; ++c)
        {
            if (forbidden[c])
                continue;
            colors[v] = c;
            if (backtrackColoring(adj, order, colors, idx + 1))
                return true;
            colors[v] = -1;
        }
        return false;
    }

    bool colorTrominoesBruteForce()
    {
        // After tiling, every covered square has a trominoId (>= 0) and the single missing
        // square keeps trominoId == -1.
        //
        // Goal: assign one of 3 colors to each tromino such that any two trominoes that share
        // a board edge (i.e., have adjacent squares) get different colors.
        //
        // Approach:
        //  1) Build an adjacency graph between tromino IDs by scanning the board.
        //  2) Use brute-force backtracking to 3-color the graph.
        //  3) Paint each square with the color of its tromino.
        const int trominoCount = nextTrominoId;
        if (trominoCount <= 0)
            return true;

        // adjSet[i] will contain all tromino IDs that touch tromino i along an edge.
        // Using a set avoids duplicate neighbors during board scanning.
        std::vector<std::unordered_set<int>> adjSet(trominoCount);
        for (int x = 0; x < width; ++x)
        {
            for (int y = 0; y < width; ++y)
            {
                const int id = tiles[x][y].getTrominoId();
                if (id < 0)
                    continue;

                // Check right neighbor (x+1, y). If it belongs to a different tromino,
                // then those two trominoes share an edge => add an undirected edge.
                if (x + 1 < width)
                {
                    const int id2 = tiles[x + 1][y].getTrominoId();
                    if (id2 >= 0 && id2 != id)
                    {
                        adjSet[id].insert(id2);
                        adjSet[id2].insert(id);
                    }
                }

                // Check bottom neighbor (x, y+1) for the same reason.
                if (y + 1 < width)
                {
                    const int id2 = tiles[x][y + 1].getTrominoId();
                    if (id2 >= 0 && id2 != id)
                    {
                        adjSet[id].insert(id2);
                        adjSet[id2].insert(id);
                    }
                }
            }
        }

        // Convert sets to vectors for faster iteration in backtracking.
        std::vector<std::vector<int>> adj(trominoCount);
        for (int i = 0; i < trominoCount; ++i)
        {
            adj[i].assign(adjSet[i].begin(), adjSet[i].end());
        }

        // Brute-force backtracking is sensitive to variable ordering.
        // Heuristic: color higher-degree trominoes first to prune earlier.
        std::vector<int> order(trominoCount);
        for (int i = 0; i < trominoCount; ++i)
            order[i] = i;
        std::sort(order.begin(), order.end(), [&](int a, int b)
                  { return adj[a].size() > adj[b].size(); });

        // trominoColors[id] in {0,1,2} means that tromino 'id' is assigned that color.
        // -1 means unassigned.
        std::vector<int> trominoColors(trominoCount, -1);
        const bool ok = backtrackColoring(adj, order, trominoColors, 0);
        if (!ok)
            return false;

        // Map the 3 integer colors to actual display characters.
        // (You can change these symbols without affecting correctness.)
        const char palette[3] = {'R', 'G', 'B'};
        for (int x = 0; x < width; ++x)
        {
            for (int y = 0; y < width; ++y)
            {
                const int id = tiles[x][y].getTrominoId();
                if (id < 0)
                {
                    // The one true missing square.
                    tiles[x][y].setColor('W');
                }
                else
                {
                    // Paint each square using its tromino's color.
                    tiles[x][y].setColor(palette[trominoColors[id]]);
                }
            }
        }
        return true;
    }

public:
    Board(int n, int missing_x, int missing_y) : width(1 << n)
    {
        // Initialize the board with empty tiles
        tiles = new Tile *[width];
        for (int i = 0; i < width; ++i)
        {
            tiles[i] = new Tile[width];
        }
        setMissingSquare(missing_x, missing_y);
    }

    ~Board() { freeTiles(); }

    // Board(const Board &) = delete;
    // Board &operator=(const Board &) = delete;

    void getMissingSquare(int &x, int &y) const
    {
        x = missing_x;
        y = missing_y;
    }

    void setMissingSquare(int x, int y)
    {
        if (x < 0 || x >= width || y < 0 || y >= width)
        {
            std::cout << "Invalid missing square coordinates. Setting to (0, 0)\n";
            missing_x = 0;
            missing_y = 0;
            return;
        }
        missing_x = x;
        missing_y = y;
    }

    void initializeBoardWithTrominos()
    {
        // 1) Divide & Conquer tiling (in-place on this board)
        nextTrominoId = 0;
        tileRec(0, 0, width, missing_x, missing_y);

        // 2) Brute force 3-coloring of the tromino adjacency graph
        if (!colorTrominoesBruteForce())
        {
            std::cout << "Failed to 3-color the tromino adjacency graph.\n";
        }
    }

    int getWidth() const { return width; }

    char getColorAt(int x, int y) const
    {
        if (x < 0 || x >= width || y < 0 || y >= width)
            return '\0';
        return tiles[x][y].getColor();
    }

    void printColors() const
    {
        for (int y = 0; y < width; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                std::cout << tiles[x][y].getColor() << ' ';
            }
            std::cout << '\n';
        }
    }
};