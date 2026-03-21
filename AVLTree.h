#pragma once
#include <string>
#include <vector>
#include <iostream>

struct SickAnimal {
    std::string name;
    int sickness; // KEY — Used for sorting within the tree
    int maxSickness; // Initial sickness level upon admission
    int stress; // Fatigue caused to volunteers
    int energy; // Speed in the turn queue
    char icon; // Representative character: 'm' for cat, 'd' for dog...
};

struct AVLNode {
    SickAnimal data;
    int key;
    AVLNode* left;
    AVLNode* right;
    int height;
};

// Function declarations for AVL tree operations
extern int g_rotationCount; // Global variable to track the number of rotations performed 
extern int g_nodeCount; // Global variable to track the number of nodes in the tree 

// AVL tree management functions
AVLNode* createNode(SickAnimal a);

// AVL tree utility functions
int avlHeight(AVLNode* node);
int avlBalanceFactor(AVLNode* node);
void avlUpdateHeight(AVLNode* node);


// Rotation and rebalancing functions
AVLNode* avlRotateRight(AVLNode* y);
AVLNode* avlRotateLeft(AVLNode* x);
AVLNode* avlRebalance(AVLNode* node);

// Insertion and deletion functions
AVLNode* avlInsert(AVLNode* node, SickAnimal a);
AVLNode* avlRemove(AVLNode* node, int key, const std::string& name);

// Search and traversal functions
AVLNode* avlfindMin(AVLNode* node);
AVLNode* avlfindMax(AVLNode* node);

// Print and Ordering functions
void avlPrintTree(AVLNode* node, int indent, bool isRight);
void avlPrintInOrder(AVLNode* node);

// Cleanup function
void avlDestroy(AVLNode* node);