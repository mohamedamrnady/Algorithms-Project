# Task01 — Tromino Tiling with 3 Colors (Algorithm Notes)

## Problem

We are given a board of size $2^n \times 2^n$ with $n>1$ and exactly one missing square. The goal is:

1. Tile every non-missing square using **right trominoes** (L-shaped tiles of 3 unit squares).
2. Use **only three colors** for trominoes.
3. Ensure that **no two trominoes that share a board edge have the same color**.

The implementation in Task01 follows a two-phase strategy:

- **Phase A (Divide & Conquer):** construct a valid tromino tiling (ignoring colors).
- **Phase B (Brute Force / Backtracking):** 3-color the tiling so adjacent trominoes differ.

---

## Phase A — Divide & Conquer tiling

### Core idea

For any $2^n \times 2^n$ board with one missing square:

- Split the board into four quadrants of size $2^{n-1} \times 2^{n-1}$.
- Exactly one quadrant contains the (real) missing square.
- Place **one** L-tromino at the center so that it covers the three center squares belonging to the **other three** quadrants.
  - This creates an **artificial “missing square”** in each of those three quadrants (because that center square is already covered).
- Recurse into each quadrant:
  - the quadrant with the real missing square keeps it;
  - the other quadrants treat their covered center square as the missing square.

This reduces the problem size by half in each dimension until the base case.

### Base case

When the region size is $2 \times 2$, the region contains exactly one missing square. One right tromino covers the remaining three squares.

### Representation used by the algorithm

Instead of placing an object representing a tromino, the tiling is stored by assigning a **tromino ID** to each covered square:

- Missing square has `trominoId = -1`.
- All 3 squares of the same tromino share the same non-negative `trominoId`.

Small snippet illustrating the base case behavior (three squares receive the same ID):

```cpp
if (width == 2) {
    const int id = nextTrominoId++;
    // assign id to the three non-missing squares
}
```

A crucial detail is that the “missing square” parameter in recursion means:

> the one square inside the sub-board that must be excluded from tiling in that call

This square can be the **real** missing square, or it can be a **pre-covered** center square.

---

## Phase B — Brute force 3-coloring of trominoes

### Why a second phase?

The divide-and-conquer tiling guarantees a correct cover of the board, but it does not enforce the coloring rule:

> If two different trominoes touch along a board edge, they must have different colors.

So we treat each tromino as a node in a graph and solve a small graph-coloring problem.

### Step 1: Build the adjacency graph

Two trominoes are adjacent if **any** square of the first is edge-adjacent (up/down/left/right) to a square of the second.

The implementation scans the grid and checks only right and bottom neighbors to add undirected edges:

- If `(x, y)` belongs to tromino `A` and `(x+1, y)` belongs to `B != A`, then add edge `A—B`.
- If `(x, y)` belongs to tromino `A` and `(x, y+1)` belongs to `B != A`, then add edge `A—B`.

This produces an adjacency list `adj[trominoId]`.

### Step 2: Backtracking with 3 colors

We assign each tromino one of three colors `{0,1,2}` by recursive backtracking:

- Choose an uncolored tromino.
- Try each of the 3 colors.
- Reject a color if any adjacent tromino already has that color.
- Continue recursively until all trominoes are colored.

To prune faster, trominoes are colored in descending degree order (higher number of neighbors first).

### Step 3: Paint squares

Once every tromino ID has a color, every square inherits its tromino’s color. The missing square stays `W`.

---

## Correctness summary (high level)

- **Tiling correctness:**
  - The divide-and-conquer step always reduces the problem to four independent subproblems, each with exactly one excluded square.
  - The base case is trivially tileable.
  - Therefore the algorithm tiles all non-missing squares.

- **Coloring correctness:**
  - The adjacency graph precisely represents “shares an edge” relationships between trominoes.
  - A valid 3-coloring guarantees that any adjacent tromino pair gets different colors.

---

## Notes on performance

Let $N = 2^n$ be the board side length. The board has $N^2$ squares and
the number of trominoes is
$$m = \frac{N^2 - 1}{3}.$$

### Phase A (Divide & Conquer tiling)

- **Time (best / average / worst):** $\Theta(N^2)$.
  - Every non-missing square is assigned exactly once to some tromino ID.
  - The recursion overhead is proportional to the number of sub-boards, which is also $\Theta(N^2)$ overall.
- **Space (extra):** $O(\log N)$ for recursion depth.
  - The board storage itself is $\Theta(N^2)$.

### Phase B (Brute force 3-coloring)

This phase has two parts: building the adjacency graph and then backtracking.

1. **Build adjacency graph**

- **Time (best / average / worst):** $\Theta(N^2)$.
  - The board is scanned once and only right/bottom neighbor edges are checked.
- **Space:** $O(m + E)$ where $E$ is the number of adjacency edges.

2. **Backtracking 3-coloring (brute force)**

- **Worst-case time:** exponential in the number of trominoes.
  - A loose upper bound is $O(3^m)$ assignments in the worst case.
  - This is expected because 3-coloring is NP-complete in general.
- **Best-case time:** close to linear in $m$ (after graph construction), when the ordering/constraints prune immediately and each tromino quickly finds a valid color.
- **Average-case time:** depends on the structure of the adjacency graph and on the chosen heuristic (here: color higher-degree trominoes first).
  - For the tilings produced by the divide-and-conquer phase, the graph is typically sparse, so backtracking often finishes quickly for small $N$.

Because Phase B can still spike in runtime, the GUI demo is capped at 16×16 to keep interaction responsive.
