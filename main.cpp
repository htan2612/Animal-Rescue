#include "include/raylib.h"
#include "AVLTree.h"
#include "MaxHeap.h"
#include <string>
#include <vector>
#include <algorithm>

// ─────────────────────────────────────────────────────────────
//  Constants
// ─────────────────────────────────────────────────────────────

static const int   SCREEN_W    = 860;
static const int   SCREEN_H    = 600;
static const int   RIGHT_W     = 220;
static const int   LEFT_W      = SCREEN_W - RIGHT_W;
static const int   HEAL_AMOUNT = 25;
static const int   MAX_LOG     = 7;
static const float FPS         = 60.0f;

// Dark theme palette
static const Color C_BG        = {15,  17,  23,  255};
static const Color C_PANEL     = {12,  14,  21,  255};
static const Color C_CARD      = {19,  22,  32,  255};
static const Color C_CARD_ACT  = {22,  26,  46,  255};
static const Color C_BORDER    = {30,  33,  48,  255};
static const Color C_BORDER_HI = {92,  107, 192, 255};
static const Color C_TEXT      = {136, 145, 176, 255};
static const Color C_MUTED     = {58,  64,  96,  255};
static const Color C_GREEN     = {76,  175, 80,  255};
static const Color C_YELLOW    = {255, 213, 79,  255};
static const Color C_ORANGE    = {255, 138, 101, 255};
static const Color C_RED       = {229, 57,  53,  255};
static const Color C_PURPLE    = {121, 134, 203, 255};
static const Color C_WHITE     = {220, 225, 240, 255};

// ─────────────────────────────────────────────────────────────
//  Game state
// ─────────────────────────────────────────────────────────────

struct LogEntry {
    std::string msg;
    Color       col;
};

struct GameState {
    AVLNode*             shelter      = nullptr;
    MaxHeap              turnQueue;
    std::vector<SickAnimal> snapshot;   // ordered list for left panel
    std::vector<LogEntry>   log;

    int  round        = 1;
    int  healed       = 0;
    int  totalRot     = 0;
    bool roundLoaded  = false;
    bool gameOver     = false;

    // which animal is currently acting (for highlight)
    std::string actingName = "";
};

// ─────────────────────────────────────────────────────────────
//  Helpers
// ─────────────────────────────────────────────────────────────

SickAnimal makeAnimal(const std::string& name, int sickness,
                      int stress, int energy, char icon) {
    SickAnimal a;
    a.name        = name;
    a.sickness    = sickness;
    a.maxSickness = sickness;
    a.stress      = stress;
    a.energy      = energy;
    a.icon        = icon;
    return a;
}

// Collect AVL tree into vector (in-order = sorted by sickness asc)
static void collectInOrder(AVLNode* node, std::vector<SickAnimal>& out) {
    if (!node) return;
    collectInOrder(node->left, out);
    out.push_back(node->data);
    collectInOrder(node->right, out);
}

// Load all animals from AVL into the turn queue (by energy)
static void loadTurnQueue(GameState& gs) {
    heapInit(gs.turnQueue);
    std::vector<SickAnimal> animals;
    collectInOrder(gs.shelter, animals);
    for (auto& a : animals) heapPush(gs.turnQueue, a);
    gs.roundLoaded = true;
    gs.actingName  = "";
}

static void addLog(GameState& gs, const std::string& msg, Color col) {
    if ((int)gs.log.size() >= MAX_LOG) gs.log.erase(gs.log.begin());
    gs.log.push_back({msg, col});
}

// Sickness → colour
static Color sickColor(int sick, int maxSick) {
    float ratio = (maxSick > 0) ? (float)sick / maxSick : 0.0f;
    if (ratio > 0.65f) return C_RED;
    if (ratio > 0.35f) return C_ORANGE;
    if (ratio > 0.10f) return C_YELLOW;
    return C_GREEN;
}

// ─────────────────────────────────────────────────────────────
//  Draw helpers
// ─────────────────────────────────────────────────────────────

static void drawPanel(int x, int y, int w, int h, Color bg, Color border) {
    DrawRectangle(x, y, w, h, bg);
    DrawRectangleLines(x, y, w, h, border);
}

static void drawLabel(const std::string& txt, int x, int y,
                      int size, Color col) {
    DrawText(txt.c_str(), x, y, size, col);
}

static void drawHLine(int x, int y, int w, Color col) {
    DrawLine(x, y, x + w, y, col);
}

// Progress bar  (x,y,w,h) filled ratio [0..1] in fillCol
static void drawBar(int x, int y, int w, int h,
                    float ratio, Color fillCol, Color bgCol) {
    DrawRectangle(x, y, w, h, bgCol);
    int filled = (int)(ratio * w);
    if (filled > 0) DrawRectangle(x, y, filled, h, fillCol);
    DrawRectangleLines(x, y, w, h, C_BORDER);
}

// ─────────────────────────────────────────────────────────────
//  Left panel — shelter animal list
// ─────────────────────────────────────────────────────────────

static void drawLeftPanel(GameState& gs) {
    // Background
    DrawRectangle(0, 0, LEFT_W, SCREEN_H, C_BG);

    // Header bar
    DrawRectangle(0, 0, LEFT_W, 36, C_PANEL);
    drawHLine(0, 36, LEFT_W, C_BORDER);

    std::string hdr = "Animal Rescue  —  Round " + std::to_string(gs.round);
    drawLabel(hdr, 14, 11, 13, C_MUTED);

    std::string hint = "SPACE: heal next   R: new round   ESC: quit";
    int hw = MeasureText(hint.c_str(), 11);
    drawLabel(hint, LEFT_W - hw - 12, 13, 11, C_MUTED);

    if (gs.snapshot.empty()) {
        drawLabel("(shelter is empty)", LEFT_W/2 - 70, SCREEN_H/2, 14, C_MUTED);
        return;
    }

    // One card per animal, sorted descending by sickness for display
    std::vector<SickAnimal> display = gs.snapshot;
    std::sort(display.begin(), display.end(),
              [](const SickAnimal& a, const SickAnimal& b){
                  return a.sickness > b.sickness;
              });

    int cardH  = 58;
    int cardGap = 6;
    int startY  = 46;

    for (int i = 0; i < (int)display.size(); i++) {
        auto& a   = display[i];
        int   cy  = startY + i * (cardH + cardGap);
        bool  act = (a.name == gs.actingName);

        Color bg  = act ? C_CARD_ACT : C_CARD;
        Color bdr = act ? C_BORDER_HI : C_BORDER;
        drawPanel(10, cy, LEFT_W - 20, cardH, bg, bdr);

        // Icon circle
        DrawCircle(34, cy + cardH/2, 16, C_PANEL);
        char iconStr[2] = {a.icon, 0};
        drawLabel(iconStr, 28, cy + cardH/2 - 8, 16, C_TEXT);

        // Name
        drawLabel(a.name, 58, cy + 10, 14, act ? C_WHITE : C_TEXT);

        // Energy badge
        std::string eBadge = "energy " + std::to_string(a.energy);
        drawLabel(eBadge, 58, cy + 28, 11, C_MUTED);

        // Sickness label + bar
        Color sc   = sickColor(a.sickness, a.maxSickness);
        float ratio = (a.maxSickness > 0)
                      ? (float)a.sickness / a.maxSickness : 0.0f;

        int barX = LEFT_W - 160;
        int barW = 120;
        drawBar(barX, cy + 18, barW, 8, ratio, sc, C_PANEL);

        std::string sickStr = std::to_string(a.sickness);
        drawLabel(sickStr, barX + barW + 8, cy + 14, 13, sc);

        // "acting" badge
        if (act) {
            drawLabel("acting", LEFT_W - 68, cy + 34, 10, C_PURPLE);
        }
    }

    // Footer buttons hint
    int fy = SCREEN_H - 34;
    DrawRectangle(0, fy, LEFT_W, 34, C_PANEL);
    drawHLine(0, fy, LEFT_W, C_BORDER);
    drawLabel("[ SPACE ] Heal next animal", 14, fy + 10, 12, C_MUTED);
    drawLabel("[ R ] Start new round", LEFT_W/2 + 20, fy + 10, 12, C_MUTED);
}

// ─────────────────────────────────────────────────────────────
//  Right panel — queue, heap, log, stats
// ─────────────────────────────────────────────────────────────

static void drawRightPanel(GameState& gs) {
    int rx = LEFT_W;
    DrawRectangle(rx, 0, RIGHT_W, SCREEN_H, C_PANEL);
    DrawLine(rx, 0, rx, SCREEN_H, C_BORDER);

    int y = 8;

    // ── Turn queue ───────────────────────────────────────────
    drawLabel("TURN QUEUE", rx + 10, y, 10, C_MUTED);
    y += 16;

    // Build display list from heap array (already sorted by energy desc)
    std::vector<SickAnimal> qList;
    for (int i = 0; i < heapSize(gs.turnQueue); i++)
        qList.push_back(gs.turnQueue.data[i]);
    std::sort(qList.begin(), qList.end(),
              [](const SickAnimal& a, const SickAnimal& b){
                  return a.energy > b.energy;
              });

    if (qList.empty()) {
        drawLabel("(empty)", rx + 10, y, 11, C_MUTED);
        y += 18;
    }

    for (int i = 0; i < (int)qList.size() && i < 5; i++) {
        auto& a   = qList[i];
        bool  top = (i == 0);
        Color bg  = top ? Color{21, 24, 42, 255} : C_CARD;
        Color bdr = top ? C_BORDER_HI : C_BORDER;
        drawPanel(rx + 6, y, RIGHT_W - 12, 28, bg, bdr);

        // Energy bar (3px wide strip on left)
        Color ec = sickColor(a.energy, 10);
        DrawRectangle(rx + 6, y, 3, 28, ec);

        char iconStr[2] = {a.icon, 0};
        drawLabel(iconStr, rx + 14, y + 7, 12, C_TEXT);
        drawLabel(a.name,  rx + 26, y + 7, 11, top ? C_WHITE : C_TEXT);

        std::string ev = "e=" + std::to_string(a.energy);
        int ew = MeasureText(ev.c_str(), 10);
        drawLabel(ev, rx + RIGHT_W - ew - 10, y + 9, 10, C_MUTED);

        y += 32;
    }

    y += 4;
    drawHLine(rx, y, RIGHT_W, C_BORDER);
    y += 8;

    // ── Heap visualisation (array indices) ───────────────────
    drawLabel("HEAP  [" + std::to_string(heapSize(gs.turnQueue)) + "]",
              rx + 10, y, 10, C_MUTED);
    y += 14;

    // Draw up to 7 nodes: row0=[0], row1=[1,2], row2=[3,4,5,6]
    int nodeW = 36, nodeH = 18, nodeGap = 4;
    int levels[3][4] = {{0,-1,-1,-1},{1,2,-1,-1},{3,4,5,6}};
    int counts[3]    = {1, 2, 4};

    for (int row = 0; row < 3; row++) {
        int cnt     = 0;
        for (int c = 0; c < counts[row]; c++)
            if (levels[row][c] < heapSize(gs.turnQueue)) cnt++;
        if (cnt == 0) break;

        int totalW  = cnt * nodeW + (cnt - 1) * nodeGap;
        int startX  = rx + (RIGHT_W - totalW) / 2;
        int drawn   = 0;

        for (int c = 0; c < counts[row]; c++) {
            int idx = levels[row][c];
            if (idx >= heapSize(gs.turnQueue)) continue;

            int nx = startX + drawn * (nodeW + nodeGap);
            Color nc = sickColor(gs.turnQueue.data[idx].energy, 10);
            DrawRectangle(nx, y, nodeW, nodeH, C_CARD);
            DrawRectangleLines(nx, y, nodeW, nodeH, nc);

            std::string val = std::to_string(gs.turnQueue.data[idx].energy);
            int vw = MeasureText(val.c_str(), 11);
            drawLabel(val, nx + (nodeW - vw)/2, y + 3, 11, nc);
            drawn++;
        }
        y += nodeH + nodeGap;
    }

    y += 6;
    drawHLine(rx, y, RIGHT_W, C_BORDER);
    y += 8;

    // ── Event log ────────────────────────────────────────────
    drawLabel("LOG", rx + 10, y, 10, C_MUTED);
    y += 14;

    for (auto& entry : gs.log) {
        // Truncate if too wide
        std::string msg = entry.msg;
        while (MeasureText(msg.c_str(), 10) > RIGHT_W - 16 && msg.size() > 4)
            msg = msg.substr(0, msg.size() - 4) + "...";
        drawLabel(msg, rx + 10, y, 10, entry.col);
        y += 13;
    }

    // ── Stats ────────────────────────────────────────────────
    int statsY = SCREEN_H - 68;
    drawHLine(rx, statsY, RIGHT_W, C_BORDER);
    statsY += 8;

    drawLabel("STATS", rx + 10, statsY, 10, C_MUTED);
    statsY += 14;

    int hw = (RIGHT_W - 18) / 2;
    // Row 1
    drawPanel(rx + 6,      statsY, hw, 34, C_CARD, C_BORDER);
    drawPanel(rx + 8 + hw, statsY, hw, 34, C_CARD, C_BORDER);

    std::string rv = std::to_string(gs.round);
    std::string hv = std::to_string(gs.healed);
    int rvw = MeasureText(rv.c_str(), 18);
    int hvw = MeasureText(hv.c_str(), 18);
    DrawText(rv.c_str(), rx + 6      + (hw - rvw)/2, statsY + 4,  18, C_PURPLE);
    DrawText(hv.c_str(), rx + 8 + hw + (hw - hvw)/2, statsY + 4,  18, C_GREEN);
    drawLabel("round",  rx + 6      + hw/2 - 14, statsY + 22, 9, C_MUTED);
    drawLabel("healed", rx + 8 + hw + hw/2 - 16, statsY + 22, 9, C_MUTED);

    statsY += 38;
    drawPanel(rx + 6,      statsY, hw, 34, C_CARD, C_BORDER);
    drawPanel(rx + 8 + hw, statsY, hw, 34, C_CARD, C_BORDER);

    std::string sv = std::to_string(g_nodeCount);
    std::string rotv = std::to_string(gs.totalRot);
    int svw   = MeasureText(sv.c_str(),   18);
    int rotvw = MeasureText(rotv.c_str(), 18);
    DrawText(sv.c_str(),   rx + 6      + (hw - svw)  /2, statsY + 4, 18, C_ORANGE);
    DrawText(rotv.c_str(), rx + 8 + hw + (hw - rotvw)/2, statsY + 4, 18, C_MUTED);
    drawLabel("in shelter", rx + 6      + hw/2 - 22, statsY + 22, 9, C_MUTED);
    drawLabel("rotations",  rx + 8 + hw + hw/2 - 20, statsY + 22, 9, C_MUTED);
}

// ─────────────────────────────────────────────────────────────
//  Game over screen
// ─────────────────────────────────────────────────────────────

static void drawGameOver(GameState& gs) {
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, C_BG);
    const char* title = "All animals healed!";
    int tw = MeasureText(title, 28);
    DrawText(title, (SCREEN_W - tw)/2, 200, 28, C_GREEN);

    std::string s1 = "Total rounds : " + std::to_string(gs.round - 1);
    std::string s2 = "AVL rotations: " + std::to_string(gs.totalRot);
    std::string s3 = "Animals healed: " + std::to_string(gs.healed);
    int w1 = MeasureText(s1.c_str(), 16);
    int w2 = MeasureText(s2.c_str(), 16);
    int w3 = MeasureText(s3.c_str(), 16);
    DrawText(s1.c_str(), (SCREEN_W - w1)/2, 260, 16, C_TEXT);
    DrawText(s2.c_str(), (SCREEN_W - w2)/2, 284, 16, C_TEXT);
    DrawText(s3.c_str(), (SCREEN_W - w3)/2, 308, 16, C_TEXT);

    const char* sub = "Press ESC to quit";
    int sw = MeasureText(sub, 13);
    DrawText(sub, (SCREEN_W - sw)/2, 360, 13, C_MUTED);
}

// ─────────────────────────────────────────────────────────────
//  Input handling
// ─────────────────────────────────────────────────────────────

static void handleInput(GameState& gs) {
    if (gs.gameOver) return;

    // R — start new round (reload turn queue)
    if (IsKeyPressed(KEY_R)) {
        if (gs.shelter != nullptr) {
            int prevRot = g_rotationCount;
            loadTurnQueue(gs);
            gs.round++;
            gs.totalRot += (g_rotationCount - prevRot);
            gs.snapshot.clear();
            collectInOrder(gs.shelter, gs.snapshot);
            addLog(gs, "Round " + std::to_string(gs.round) + " started", C_PURPLE);
        }
        return;
    }

    // SPACE — heal next animal in turn queue
    if (IsKeyPressed(KEY_SPACE)) {
        if (!gs.roundLoaded) {
            loadTurnQueue(gs);
            gs.snapshot.clear();
            collectInOrder(gs.shelter, gs.snapshot);
            addLog(gs, "Round " + std::to_string(gs.round) + " loaded", C_PURPLE);
            return;
        }

        if (heapIsEmpty(gs.turnQueue)) {
            addLog(gs, "Queue empty — press R for next round", C_YELLOW);
            return;
        }

        int prevRot   = g_rotationCount;
        SickAnimal actor = heapPop(gs.turnQueue);
        gs.actingName    = actor.name;

        int oldSick   = actor.sickness;
        actor.sickness = std::max(0, actor.sickness - HEAL_AMOUNT);

        gs.shelter = avlRemove(gs.shelter, oldSick, actor.name);

        if (actor.sickness > 0) {
            gs.shelter = avlInsert(gs.shelter, actor);
            addLog(gs,
                actor.name + ": " + std::to_string(oldSick)
                + " -> " + std::to_string(actor.sickness),
                sickColor(actor.sickness, actor.maxSickness));
        } else {
            gs.healed++;
            addLog(gs, actor.name + " fully healed!", C_GREEN);
            gs.actingName = "";
        }

        gs.totalRot += (g_rotationCount - prevRot);

        // Refresh snapshot
        gs.snapshot.clear();
        collectInOrder(gs.shelter, gs.snapshot);

        if (gs.shelter == nullptr) {
            gs.gameOver = true;
        }
    }
}

// ─────────────────────────────────────────────────────────────
//  Entry point
// ─────────────────────────────────────────────────────────────

int main() {
    // ── Init shelter ─────────────────────────────────────────
    GameState gs;
    g_rotationCount = 0;
    g_nodeCount     = 0;
    heapInit(gs.turnQueue);

    gs.shelter = avlInsert(gs.shelter, makeAnimal("Mochi",   75, 12, 5, 'm'));
    gs.shelter = avlInsert(gs.shelter, makeAnimal("Biscuit", 40,  8, 8, 'd'));
    gs.shelter = avlInsert(gs.shelter, makeAnimal("Pepper",  90, 18, 3, 'p'));
    gs.shelter = avlInsert(gs.shelter, makeAnimal("Sunny",   55, 10, 6, 's'));
    gs.shelter = avlInsert(gs.shelter, makeAnimal("Coco",    20,  5, 9, 'c'));

    collectInOrder(gs.shelter, gs.snapshot);
    loadTurnQueue(gs);
    addLog(gs, "Shelter opened — 5 animals admitted", C_GREEN);
    addLog(gs, "SPACE to heal  |  R for new round", C_MUTED);

    // ── Raylib window ─────────────────────────────────────────
    InitWindow(SCREEN_W, SCREEN_H, "Animal Rescue Shelter  --  Day 3");
    SetTargetFPS((int)FPS);

    while (!WindowShouldClose()) {
        handleInput(gs);

        BeginDrawing();
        ClearBackground(C_BG);

        if (gs.gameOver) {
            drawGameOver(gs);
        } else {
            drawLeftPanel(gs);
            drawRightPanel(gs);
        }

        EndDrawing();
    }

    avlDestroy(gs.shelter);
    CloseWindow();
    return 0;
}