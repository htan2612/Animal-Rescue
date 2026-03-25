#include <iostream>
#include <string>

#include "AVLTree.h"
#include "MaxHeap.h"

SickAnimal makeAnimal(std::string name, int sickness, int stress, int energy, char icon) {
    return {name, sickness, sickness, stress, energy, icon};
}

static void loadIntoHeap(AVLNode* node, MaxHeap& h) {
    if (node == nullptr) return;
    loadIntoHeap(node->left, h);
    heapPush(h, node->data);
    loadIntoHeap(node->right, h);
}

static void divider(const std::string& title = "") {
    std::string line(30, '=');
    std::cout << "\n" << line << (title.empty() ? "" : "\n " + title + "\n" + line) << "\n";
}

// Game loop and setup
int main() {
    AVLNode* shelter = nullptr;
    g_rotationCount = g_nodeCount = 0;

    shelter = avlInsert(shelter, makeAnimal("Mochi", 75, 12, 5, 'm'));
    shelter = avlInsert(shelter, makeAnimal("Biscuit", 40, 8, 8, 'd'));
    shelter = avlInsert(shelter, makeAnimal("Pepper", 90, 18, 3, 'p'));
    shelter = avlInsert(shelter, makeAnimal("Sunny", 55, 10, 6, 's'));
    shelter = avlInsert(shelter, makeAnimal("Coco", 20, 5, 9, 'c'));

    divider("Animal Rescue Shelter — Day 2");
    std::cout << "Admitted: " << g_nodeCount << " animals\n";
    avlPrintTree(shelter, 0, false);

    // Game parameters
    const int HEAL_AMOUNT = 25;
    int round = 1;

    while (shelter != nullptr) {
        divider("Round " + std::to_string(round));

        // Step 1: Load all animals into the turn queue (by energy)
        MaxHeap turnQueue;
        heapInit(turnQueue);
        loadIntoHeap(shelter, turnQueue);

        std::cout << "\nTurn order (highest energy acts first):\n";
        heapPrint(turnQueue);

        // Step 2: Each animal takes its turn
        std::cout << "\n-- Actions --\n";
        while (!heapIsEmpty(turnQueue)) {
            SickAnimal actor = heapPop(turnQueue);

            std::cout << "\n>> " << actor.icon << " " << actor.name
                      << " acts  (energy=" << actor.energy << ")\n";

            int oldSickness = actor.sickness;
            actor.sickness  = actor.sickness - HEAL_AMOUNT;
            if (actor.sickness < 0) actor.sickness = 0;

            std::cout << "   Volunteer heals: sickness "
                      << oldSickness << " -> " << actor.sickness << "\n";

            // Remove old record, re-insert with updated sickness
            shelter = avlRemove(shelter, oldSickness, actor.name);

            if (actor.sickness > 0) {
                shelter = avlInsert(shelter, actor);
                std::cout << "   Still sick — re-admitted to shelter.\n";
            } else {
                std::cout << "   Fully healed! " << actor.icon
                          << " " << actor.name << " goes home.\n";
            }
        }

        // Step 3: Show shelter state after the round
        std::cout << "\nShelter after round " << round << "  ("
                  << g_nodeCount << " animals remaining):\n";

        if (shelter != nullptr) {
            avlPrintTree(shelter, 0, false);
        } else {
            std::cout << "  (shelter is empty)\n";
        }

        round++;

        // Safety cap — should not be needed with correct logic
        if (round > 20) {
            std::cout << "\n[game] Round limit reached — exiting.\n";
            break;
        }
    }

    divider("All animals healed!");
    std::cout << " Total rounds : " << round - 1 << "\n";
    std::cout << " AVL rotations: " << g_rotationCount << "\n\n";

    avlDestroy(shelter);
    return 0;
}