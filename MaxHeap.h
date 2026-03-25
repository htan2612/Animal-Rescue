#pragma once
#include "AVLTree.h"

#define MAX_HEAP_SIZE 256

struct MaxHeap {
    SickAnimal data[MAX_HEAP_SIZE];
    int size;
};

// Heap management functions
void heapInit(MaxHeap& h); 

// Heap utility functions
void heapPush(MaxHeap& h, SickAnimal a);
SickAnimal heapPop(MaxHeap& h);
SickAnimal heapPeek(const MaxHeap& h);

// Heap status functions
bool heapIsEmpty(const MaxHeap& h);
int heapSize(const MaxHeap& h);
void heapPrint(const MaxHeap& h);

// Heap internal functions
int heapParent(int i);
int heapLeftChild(int i);
int heapRightChild(int i);