# 🐾 Animal Rescue Shelter

A small console game where volunteers heal sick animals based on priority.  
Built to practice **AVL Tree** and **Max-Heap** in C++17 — everything coded from scratch, no STL containers.

---

## How to Run

```bash
# Step 1 — compile all files
g++ -std=c++17 -g AVLTree.cpp MaxHeap.cpp main.cpp -o rescue.exe

# Step 2 — run
./rescue.exe
```

Or press `Ctrl+Shift+B` in VS Code (uses the `build project` task).

---

## What It Does

Sick animals are stored in an **AVL Tree** sorted by sickness level.  
Each round, all animals get loaded into a **Max-Heap** sorted by energy — higher energy acts first.  
A volunteer heals each animal by 25 sickness points per turn.  
When an animal hits 0 sickness, it leaves the shelter. The game ends when everyone is healed.

---

## Progress

- [x] **Day 1** — AVL Tree
- [x] **Day 2** — Max-Heap + console game loop


---

## File Structure

```
Animal-Rescue/
├── AVLTree.h       ← structs + function prototypes
├── AVLTree.cpp     ← AVL tree implementation
├── MaxHeap.h       ← heap struct + prototypes
├── MaxHeap.cpp     ← heap implementation
├── main.cpp        ← game loop
└── README.md
```

---

## AVL Tree

Stores animals sorted by `sickness` (the key).  
Self-balances after every insert and remove using rotations.

| Function | Description | Time |
|---|---|---|
| `avlInsert(root, animal)` | Add animal to tree | O(log n) |
| `avlRemove(root, key, name)` | Remove animal by sickness + name | O(log n) |
| `avlfindMin(root)` | Least sick animal | O(log n) |
| `avlfindMax(root)` | Most sick animal | O(log n) |
| `avlPrintTree(root, 0, false)` | Print tree sideways | O(n) |
| `avlPrintInOrder(root)` | Print sorted list | O(n) |

**Why AVL and not just an array?**  
Each turn needs 3 steps: find the animal, remove it (sickness changes so the key changes), then re-insert.  
With an array that's O(n) per step. AVL Tree keeps all 3 at O(log n) because it stays balanced.

**Why not `std::map`?**  
`std::map` is a Red-Black Tree under the hood — also O(log n), but you can't see how it works.  
Writing AVL from scratch makes the rotation logic visible, which is the point of this project.

---

## Max-Heap

A fixed-size array-based max-heap sorted by `energy`.  
Used as a turn queue each round — highest energy animal acts first.

| Function | Description |
|---|---|
| `heapPush(heap, animal)` | Add animal to heap |
| `heapPop(heap)` | Remove and return top animal |
| `heapPeek(heap)` | Look at top animal without removing |
| `heapIsEmpty(heap)` | Check if heap is empty |
| `heapPrint(heap)` | Print all animals in heap order |

---

## Sample Output

```
==============================
 Animal Rescue Shelter — Day 2
==============================
Admitted: 5 animals

          /--- p Pepper [sick:90 h:1]
     /--- s Sunny [sick:55 h:2]
m Mochi [sick:75 h:3]
     \--- d Biscuit [sick:40 h:1]
          \--- c Coco [sick:20 h:1]

==============================
 Round 1
==============================

Turn order (highest energy acts first):
  [0] c Coco    energy=9  sick=20
  [1] d Biscuit energy=8  sick=40
  ...
```

---

## Notes

- Duplicate sickness keys are handled — `avlRemove` matches both key and name.
- `g_rotationCount` and `g_nodeCount` are global counters tracked across all operations.
- Max heap size is 256 (`MAX_HEAP_SIZE` in MaxHeap.h), which is enough for this game.