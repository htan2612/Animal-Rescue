#include "AVLTree.h"
#include <string>

int g_rotationCount = 0;
int g_nodeCount = 0;

AVLNode* createNode(SickAnimal a) {
    AVLNode* node = new AVLNode();
    node->data = a;
    node->key = a.sickness;
    node->left = nullptr;
    node->right = nullptr;
    node->height = 1;
    return node;
}

int avlHeight(AVLNode* node) {
    if (node == nullptr) return 0;
    return node->height;
}

int avlBalanceFactor(AVLNode* node) {
    if (node == nullptr) return 0;
    return avlHeight(node->left) - avlHeight(node->right);
}

void avlUpdateHeight(AVLNode* node) {
    if (node != nullptr) {
        int leftHeight = avlHeight(node->left);
        int rightHeight = avlHeight(node->right);
        if (leftHeight > rightHeight) {
            node->height = leftHeight + 1;
        } else {
            node->height = rightHeight + 1;
        }
    }
}

AVLNode* avlRotateRight(AVLNode* y) {
    g_rotationCount++;

    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    avlUpdateHeight(y);
    avlUpdateHeight(x);

    return x; // New root
}

AVLNode* avlRotateLeft(AVLNode* x) {
    g_rotationCount++;

    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update heights
    avlUpdateHeight(x);
    avlUpdateHeight(y);

    return y; // New root
}

AVLNode* avlRebalance(AVLNode* node) {
    avlUpdateHeight(node);

    int balance = avlBalanceFactor(node);

    // Left Left Case
    if (balance > 1 && avlBalanceFactor(node->left) >= 0) {
        return avlRotateRight(node);
    }

    // Left Right Case
    if (balance > 1 && avlBalanceFactor(node->left) < 0) {
        node->left = avlRotateLeft(node->left);
        return avlRotateRight(node);
    }

    // Right Right Case
    if (balance < -1 && avlBalanceFactor(node->right) <= 0) {
        return avlRotateLeft(node);
    }

    // Right Left Case
    if (balance < -1 && avlBalanceFactor(node->right) > 0) {
        node->right = avlRotateRight(node->right);
        return avlRotateLeft(node);
    }

    return node;
}

AVLNode* avlInsert(AVLNode* node, SickAnimal a) {
    if (node == nullptr) {
        g_nodeCount++;
        return createNode(a);
    }

    if (a.sickness < node->key) {
        node->left = avlInsert(node->left, a);
    } else {
        node->right = avlInsert(node->right, a);
    }

    return avlRebalance(node);
}

AVLNode* avlfindMin(AVLNode* node) {
    if (node->left == nullptr) {
        return node;
    }
    else {
        return avlfindMin(node->left);
    }
}

AVLNode* avlfindMax(AVLNode* node) {
    if (node->right == nullptr) {
        return node;
    }
    else {
        return avlfindMax(node->right);
    }
}

AVLNode* avlRemove(AVLNode* node, int key, const std::string& name) {
    if (node == nullptr) return nullptr;

    if (key < node->key) {
        node->left = avlRemove(node->left, key, name);
    } 
    else if (key > node->key) {
        node->right = avlRemove(node->right, key, name);
    }
    else if (node->data.name != name) {
        // If names don't match but keys do (duplicate keys), 
        // continue searching in the subtree where duplicates might exist
        node->right = avlRemove(node->right, key, name);
    }
    else {
        if (node->left == nullptr || node->right == nullptr) {
            // Case: One child or No child
            AVLNode* temp = node->left ? node->left : node->right;
            
            if (temp == nullptr) { // No child
                temp = node;
                node = nullptr;
            } else { // One child
                *node = *temp; // Copy contents of the non-empty child
            }
            
            delete temp;
            g_nodeCount--;
        } else {
            // Case: Two children
            AVLNode* successor = avlfindMin(node->right);
            
            // Copy successor's data to this node
            node->data = successor->data;
            node->key = successor->key;

            // Delete the successor
            node->right = avlRemove(node->right, successor->key, successor->data.name);
        }
    }

    // 4. If tree had only one node, return
    if (node == nullptr) return nullptr;

    // 5. Update height and rebalance
    return avlRebalance(node);
}

void avlPrintTree(AVLNode* node, int indent, bool isRight) {
    if (node == nullptr) return;

    // Print right subtree first (higher keys)
    avlPrintTree(node->right, indent + 5, true);

    // Print current node
    for (int i = 0; i < indent; i++) std::cout << ' ';

    if (indent > 0) {
        if (isRight == true) {
            std::cout << "/--- ";
        } else {
            std::cout << "\\--- ";
        }
    }

    std::cout << node->data.icon << " "
              << node->data.name
              << " [sick:" << node->data.sickness
              << " h:" << node->height << "]\n";

    // In cây con trái sau
    avlPrintTree(node->left, indent + 5, false);
}

void avlPrintInOrder(AVLNode* node) {
    if (node == nullptr) {
        return;
    }
    avlPrintInOrder(node->left);
    std::cout << node->data.icon << " "
              << node->data.name
              << "  sick=" << node->data.sickness << "\n";
    avlPrintInOrder(node->right);
}

void avlDestroy(AVLNode* node) {
    if (node == nullptr) return;
    avlDestroy(node->left);
    avlDestroy(node->right);    
    delete node;
}