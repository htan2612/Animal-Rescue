# 🐾 Animal Rescue Shelter

A game where volunteers heal sick animals based on priority.  
Built to practice **AVL Tree** and **Max-Heap** in C++17 — everything coded from scratch, no STL containers.

---

## How to Run

### Requirements
- [MSYS2](https://www.msys2.org/) with the **UCRT64** toolchain
- Raylib installed via pacman

### First-time setup

Open **MSYS2 UCRT64** and run:

```bash
pacman -S mingw-w64-ucrt-x86_64-raylib
```

### Compile

```bash
cd "/c/Users/YOUR_NAME/Downloads/Animal Rescue"
g++ -std=c++17 -o game AVLTree.cpp MaxHeap.cpp main.cpp -lraylib -lopengl32 -lgdi32 -lwinmm
```

### Run

```bash
./game.exe
```

> Always compile and run from **MSYS2 UCRT64** terminal.  
> Double-clicking `game.exe` in File Explorer will not work.

---

## Controls

| Key | Action |
|---|---|
| `SPACE` | Heal the next animal in the turn queue |
| `R` | Start a new round |
| `ESC` | Quit |

---

## How It Works

Sick animals are stored in an **AVL Tree** sorted by sickness level.  
Each round, all animals are loaded into a **Max-Heap** sorted by energy — highest energy acts first.  
A volunteer heals each animal by 25 sickness points per turn.  
When an animal reaches 0 sickness, it leaves the shelter.  
The game ends when all animals are healed.

---

## Progress

- [x] **Day 1** — AVL Tree
- [x] **Day 2** — Max-Heap + console game loop
- [x] **Day 3** — Raylib window + UI

---

## File Structure

```
Animal-Rescue/
├── AVLTree.h       ← structs + function prototypes
├── AVLTree.cpp     ← AVL tree implementation
├── MaxHeap.h       ← heap struct + prototypes
├── MaxHeap.cpp     ← heap implementation
├── main.cpp        ← Raylib game loop + UI
└── README.md
```

---

## AVL Tree

Stores animals sorted by `sickness` (the key).  
Self-balances after every insert and remove using rotations.

| Function | Description | Complexity |
|---|---|---|
| `avlInsert(root, animal)` | Add animal to tree | O(log n) |
| `avlRemove(root, key, name)` | Remove animal by sickness + name | O(log n) |
| `avlfindMin(root)` | Least sick animal | O(log n) |
| `avlfindMax(root)` | Most sick animal | O(log n) |
| `avlPrintTree(root, 0, false)` | Print tree sideways | O(n) |
| `avlPrintInOrder(root)` | Print sorted list | O(n) |

**Why AVL and not just an array?**  
Each turn needs 3 steps: find the animal, remove it (sickness changes so the key changes), then re-insert.  
With an array that is O(n) per step. AVL Tree keeps all 3 at O(log n) because it stays balanced.

**Why not `std::map`?**  
`std::map` is a Red-Black Tree under the hood — also O(log n), but you cannot see how it works.  
Writing AVL from scratch makes the rotation logic visible, which is the point of this project.

---

## Max-Heap

A fixed-size array-based max-heap sorted by `energy`.  
Used as a turn queue each round — highest energy animal acts first.

| Function | Description | Complexity |
|---|---|---|
| `heapPush(heap, animal)` | Add animal to heap | O(log n) |
| `heapPop(heap)` | Remove and return top animal | O(log n) |
| `heapPeek(heap)` | Look at top without removing | O(1) |
| `heapIsEmpty(heap)` | Check if heap is empty | O(1) |
| `heapPrint(heap)` | Print all animals in heap order | O(n) |

**Why Max-Heap for the turn queue?**  
The turn queue is rebuilt every round — all animals pushed in, then popped out in energy order.  
This is exactly what a heap is optimised for: N insertions + N deletions in O(n log n) total.

**Index formula (array-based heap):**
```
parent(i)     = (i - 1) / 2
leftChild(i)  = 2 * i + 1
rightChild(i) = 2 * i + 2
```

---

## Notes

- Duplicate sickness keys are handled — `avlRemove` matches both key and name.
- `g_rotationCount` and `g_nodeCount` are global counters tracked across all operations.
- Max heap size is 256 (`MAX_HEAP_SIZE` in `MaxHeap.h`).