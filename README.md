# 🐾 Animal Rescue Shelter

> Volunteers heal sick animals based on priority —
> a hands-on demonstration of **AVL Tree** and **Max-Heap Priority Queue** in C++17.
> All data structures are implemented from scratch without STL containers.

---

## Progress

- [x] **Day 1** — AVL Tree
- [ ] Day 2 — Max-Heap + console game
- [ ] Day 3 — Raylib window + layout
- [ ] Day 4 — AVL Tree graphics + volunteer cards
- [ ] Day 5 — Polish + GitHub showcase

---

## Day 1 — AVL Tree

The AVL Tree stores sick animals sorted by **sickness level**.
Volunteers always treat the most critical animal first — `avlfindMax()`.

### File Structure

```
Animal-Rescue/
├── AVLTree.h       ← struct declarations + function prototypes
├── AVLTree.cpp     ← implementation
└── README.md
```

### Functions

| Function | Description | Complexity |
|---|---|---|
| `avlInsert(root, animal)` | Add animal to tree | O(log n) |
| `avlRemove(root, key, name)` | Remove animal from tree | O(log n) |
| `avlfindMax(root)` | Find most critical animal | O(log n) |
| `avlfindMin(root)` | Find least critical animal | O(log n) |
| `avlPrintTree(root, 0, false)` | Print tree sideways to console | O(n) |
| `avlPrintInOrder(root)` | Print sorted list ascending | O(n) |

### Why AVL Tree?

Each healing turn requires 3 operations:
1. `avlfindMax()` — find the sickest animal
2. `avlRemove()` — remove it (sickness level changes → must remove)
3. `avlInsert()` — re-insert with updated sickness level

With a plain array: findMax is O(n), remove is O(n) — too slow with many animals.
AVL Tree guarantees all 3 operations in **O(log n)** in every case,
because the tree self-balances after every insert and remove.

### Why not `std::map`?

`std::map` uses a Red-Black Tree internally — also O(log n), but a black-box.
Writing AVL Tree from scratch makes the rotation logic visible and explainable,
which is the whole point of this project.

### Compile & Run

```bash
g++ -std=c++17 -o test AVLTree.cpp test_avl.cpp && ./test
```

### Sample Output

```
=============================
 TEST 1: Insert 4 con
=============================
          /--- v Vet1 [sick:90 h:1]
     /--- d Cho1 [sick:70 h:2]
m Meo1 [sick:45 h:3]
     \--- r Tho1 [sick:30 h:1]
So node : 4
Rotation: 0
```

---
