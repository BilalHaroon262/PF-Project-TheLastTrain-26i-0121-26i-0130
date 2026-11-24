#include "logger.h"
#include "util.h"
#include "level.h"
#include <fstream>
#include <iostream>
using namespace std;

// File persistence for savegame.txt and game_summary.txt using primitive variables
bool saveGameStateToFile(const char* filename,
                         int playerFloor, int playerRoom,
                         int energy, int time, int score,
                         const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount,
                         const bool objectives[NUM_OBJECTIVES], int emergencyTripsLeft,
                         const char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                         const int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR]) {
    if (!filename) {
        return false; }
    ofstream out (filename);
    if (!out.is_open()) {
        return false; }
    out << "STATE_FLOOR " << playerFloor << endl;
    out << "STATE_ROOM " << playerRoom << endl;
    out << "STATE_ENERGY " << energy << endl;
    out << "STATE_TIME " << time << endl;
    out << "STATE_SCORE " << score << endl;
    out << "STATE_TRIPS " << emergencyTripsLeft << endl;
    int safeCount = clampInt(inventoryCount,0,MAX_INVENTORY_CAPACITY);
    out << "BAG_SIZE " << safeCount << endl;
    out << "BAG_ITEMS";
    for (int i = 0; i < safeCount; i++) {
        out << " " << inventory[i]; }
    out << endl;
    out << "FLAGS";
    for (int j = 0; j < NUM_OBJECTIVES; j++) {
        out << " " << (objectives[j] ? 1 : 0); }
    out << endl;
    for (int f = 0; f < NUM_FLOORS; f++) {
        for (int r = 0; r < ROOMS_PER_FLOOR; r++) {
            int count = clampInt(entityCounts[f][r],0,MAX_ENTITIES_PER_ROOM);
            out << "CELL " << (f + 1) << " " << (r + 1) << " " << count;
            for (int e = 0; e < count && e < MAX_ENTITIES_PER_ROOM; e++) {
                out << " " << roomEntities[f][r][e]; }
            out << endl;
        } 
    }
    out.close();
    return !out.fail();
}

