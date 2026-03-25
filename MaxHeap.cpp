#include "MaxHeap.h"
#include <iostream>

int heapParent(int i) {
    return (i-1)/2;
}
int heapLeftChild(int i) {
    return 2*i + 1;
}
int heapRightChild(int i) {
    return 2*i + 2;
}

void heapInit(MaxHeap& h) {
    h.size = 0;
}

// Use when add animal to the heap
static void heapifyUp(MaxHeap& h, int i) {
    while (i > 0) {
        int p = heapParent(i);
        if (h.data[p].energy < h.data[i].energy) {
            SickAnimal tmp = h.data[p];
            h.data[p] = h.data[i];
            h.data[i] = tmp;
            i = p;
        } else break;
    }
}

// Use when remove animal from the heap
static void heapifyDown(MaxHeap& h, int i) {
    while (true) {
        int largest = i;
        int l = heapLeftChild(i);
        int r = heapRightChild(i);

        if(l < h.size && h.data[l].energy > h.data[largest].energy) {
            largest = l;
        }
        if(r < h.size && h.data[r].energy > h.data[largest].energy) {
            largest = r;
        }

        if (largest != i) {
            SickAnimal tmp = h.data[i];
            h.data[i] = h.data[largest];
            h.data[largest] = tmp;
            i = largest;
        } else break;
    }
}

// Priority Queue
// Use when add animal to the heap
void heapPush(MaxHeap& h, SickAnimal a) {
    if (h.size >= MAX_HEAP_SIZE) {
        std::cerr << "Heap is full! Cannot add more animals.\n";
        return;
    }

    h.data[h.size] = a;
    heapifyUp(h, h.size);
    h.size++;
}

// Use when remove animal from the heap
SickAnimal heapPop(MaxHeap& h) {
    if (h.size <= 0) {
        std::cerr << "Heap is empty! Cannot pop animal.\n";
        return SickAnimal(); // Return default animal
    }

    SickAnimal top = h.data[0];
    h.data[0] = h.data[h.size - 1];
    h.size--;
    heapifyDown(h, 0);

    return top;
}

// Use when peek animal at the top of the heap without removing it
SickAnimal heapPeek(const MaxHeap& h) {
    if (h.size <= 0) {
        std::cerr << "Heap is empty! Cannot peek animal.\n";
        return SickAnimal(); // Return default animal
    }
    return h.data[0];
}

bool heapIsEmpty(const MaxHeap& h) {
    return h.size == 0;
}
int heapSize(const MaxHeap& h) {
    return h.size;
}

// Use when check if the heap is empty
void heapPrint(const MaxHeap& h) {
    if (h.size == 0) {
        std::cout << "  (empty heap)\n";
        return;
    }
    for (int i = 0; i < h.size; i++) {
        std::cout << "  [" << i << "] "
                  << h.data[i].icon << " "
                  << h.data[i].name
                  << "  energy=" << h.data[i].energy
                  << "  sick="   << h.data[i].sickness
                  << "\n";
    }
    return;
}   