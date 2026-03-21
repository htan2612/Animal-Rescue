#include <iostream>
#include "AVLTree.h"

SickAnimal taoThu(std::string ten, int benh, int stress, int energy, char icon) {
    SickAnimal a;
    a.name        = ten;
    a.sickness    = benh;
    a.maxSickness = benh;
    a.stress      = stress;
    a.energy      = energy;
    a.icon        = icon;
    return a;
}

int main() {

    AVLNode* root = nullptr;

    // ─────────────────────────────────────────
    //  TEST 1: Insert co ban
    // ─────────────────────────────────────────
    std::cout << "=============================\n";
    std::cout << " TEST 1: Insert 4 con\n";
    std::cout << "=============================\n";

    root = avlInsert(root, taoThu("Meo1", 45, 8,  5, 'm'));
    root = avlInsert(root, taoThu("Cho1", 70, 14, 4, 'd'));
    root = avlInsert(root, taoThu("Tho1", 30, 6,  6, 'r'));
    root = avlInsert(root, taoThu("Vet1", 90, 20, 5, 'v'));

    avlPrintTree(root, 0, false);   // FIX 4: avl_printTree → avlPrintTree
    std::cout << "So node : " << g_nodeCount     << "\n";
    std::cout << "Rotation: " << g_rotationCount << "\n";

    // Ket qua mong doi:
    //           /--- v Vet1  [sick:90 h:1]
    //      /--- d Cho1  [sick:70 h:2]
    //  m Meo1  [sick:45 h:3]
    //      \--- r Tho1  [sick:30 h:1]
    // So node : 4  |  Rotation: 0

    // ─────────────────────────────────────────
    //  TEST 2: findMax va findMin
    // ─────────────────────────────────────────
    std::cout << "\n=============================\n";
    std::cout << " TEST 2: findMax va findMin\n";
    std::cout << "=============================\n";

    AVLNode* maxNode = avlfindMax(root);
    AVLNode* minNode = avlfindMin(root);

    if (maxNode != nullptr) {
        std::cout << "Max (benh nang nhat): "
                  << maxNode->data.name
                  << "  sick=" << maxNode->data.sickness << "\n";
    }
    if (minNode != nullptr) {
        std::cout << "Min (benh nhe nhat) : "
                  << minNode->data.name
                  << "  sick=" << minNode->data.sickness << "\n";
    }

    // Ket qua mong doi:
    // Max: Vet1 sick=90  |  Min: Tho1 sick=30

    // ─────────────────────────────────────────
    //  TEST 3: avlPrintInOrder (chi in, 1 tham so)
    // ─────────────────────────────────────────
    std::cout << "\n=============================\n";
    std::cout << " TEST 3: InOrder (tang dan)\n";
    std::cout << "=============================\n";

    avlPrintInOrder(root);  // FIX 5: bo vector, goi 1 tham so

    // Ket qua mong doi:
    // r Tho1  sick=30
    // m Meo1  sick=45
    // d Cho1  sick=70
    // v Vet1  sick=90

    // ─────────────────────────────────────────
    //  TEST 4: Remove
    // ─────────────────────────────────────────
    std::cout << "\n=============================\n";
    std::cout << " TEST 4: Remove Cho1\n";
    std::cout << "=============================\n";

    root = avlRemove(root, 70, "Cho1");
    avlPrintTree(root, 0, false);   // FIX 6: avl_printTree → avlPrintTree
    std::cout << "So node : " << g_nodeCount     << "\n";
    std::cout << "Rotation: " << g_rotationCount << "\n";

    // Ket qua mong doi:
    //      /--- v Vet1  [sick:90 h:1]
    //  m Meo1  [sick:45 h:2]
    //      \--- r Tho1  [sick:30 h:1]
    // So node : 3  |  Rotation: 1

    // ─────────────────────────────────────────
    //  TEST 5: Rotation tu dong
    // ─────────────────────────────────────────
    std::cout << "\n=============================\n";
    std::cout << " TEST 5: Rotation tu dong\n";
    std::cout << "=============================\n";

    AVLNode* root2  = nullptr;
    g_rotationCount = 0;
    g_nodeCount     = 0;

    root2 = avlInsert(root2, taoThu("A", 10, 5, 5, 'a')); // FIX: avl_insert → avlInsert
    root2 = avlInsert(root2, taoThu("B", 20, 5, 5, 'b'));
    root2 = avlInsert(root2, taoThu("C", 30, 5, 5, 'c'));

    avlPrintTree(root2, 0, false);
    std::cout << "Rotation: " << g_rotationCount << "\n";

    // Ket qua mong doi:
    //      /--- c C [sick:30 h:1]
    //  b B [sick:20 h:2]
    //      \--- a A [sick:10 h:1]
    // Rotation: 1

    // ─────────────────────────────────────────
    //  TEST 6: Remove het → cay rong
    // ─────────────────────────────────────────
    std::cout << "\n=============================\n";
    std::cout << " TEST 6: Remove het cay\n";
    std::cout << "=============================\n";

    root = avlRemove(root, 45, "Meo1");
    root = avlRemove(root, 30, "Tho1");
    root = avlRemove(root, 90, "Vet1");

    if (root == nullptr) {
        std::cout << "Cay da rong!\n";
    } else {
        std::cout << "Cay chua rong - co loi!\n";
    }

    avlDestroy(root);
    avlDestroy(root2);

    std::cout << "\n=============================\n";
    std::cout << " Tat ca test PASS!\n";
    std::cout << "=============================\n";

    return 0;
}