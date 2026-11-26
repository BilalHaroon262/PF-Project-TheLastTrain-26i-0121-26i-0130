#include "logger.h"
#include "util.h"
#include "level.h"
#include <fstream>
#include <iostream>
#include <iomanip>
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
            for (int e = 0; e < count; e++) {
                out << " " << roomEntities[f][r][e]; }
            out << endl;
        } 
    }
    out.close();
    return !out.fail();
}
bool loadGameStateFromFile(const char* filename,
                           int& playerFloor, int& playerRoom,
                           int& energy, int& time, int& score,
                           char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
                           bool objectives[NUM_OBJECTIVES], int& emergencyTripsLeft,
                           char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                           int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR]) {
    if (!filename) {
        return false; }
    ifstream in (filename);
    if (!in.is_open()) {
        return false; }
    char tag[MAX_LINE_LEN];
    while (in >> setw(MAX_LINE_LEN) >> tag) {
        if (stringEquals(tag,"STATE_FLOOR")) {
            in >> playerFloor; }
        else if (stringEquals(tag,"STATE_ROOM")) {
            in >> playerRoom; }
        else if (stringEquals(tag,"STATE_ENERGY")) {
            in >> energy; }
        else if (stringEquals(tag,"STATE_TIME")) {
            in >> time; }
        else if (stringEquals(tag,"STATE_SCORE")) {
            in >> score; }
        else if (stringEquals(tag,"STATE_TRIPS")) {
            in >> emergencyTripsLeft; }
        else if (stringEquals(tag,"BAG_SIZE")) {
            in >> inventoryCount;
            inventoryCount = clampInt(inventoryCount,0,MAX_INVENTORY_CAPACITY); }
        else if (stringEquals(tag, "BAG_ITEMS")) {
            for (int i = 0; i < MAX_INVENTORY_CAPACITY; i++)
                inventory[i] = ENT_NONE;
            for (int i = 0; i < inventoryCount; i++) 
                in >> inventory[i]; }
        else if (stringEquals(tag,"FLAGS")) {
            for (int j = 0; j < NUM_OBJECTIVES; j++) {
                int v = 0;
                in >> v;
                objectives[j] = (v == 1); } 
            }
        else if (stringEquals(tag,"CELL")) {
            int fl=0, rm=0, n=0;
            in >> fl >> rm >> n;
            if (fl >= 1 && fl <= NUM_FLOORS && rm >= 1 && rm <= ROOMS_PER_FLOOR) {
                n = clampInt(n,0,MAX_ENTITIES_PER_ROOM);
                entityCounts[fl - 1][rm - 1] = n;
                for (int e = 0; e < MAX_ENTITIES_PER_ROOM; e++)
                    roomEntities[fl-1][rm-1][e] = ENT_NONE;
                for (int e = 0; e < n; e++) 
                    in >> roomEntities[fl-1][rm-1][e]; }
            }
        }
        in.close();
        return true;
}
void writeGameSummaryReport(const char* filename,
                            int score, int remainingEnergy, int remainingTime,
                            const bool objectives[NUM_OBJECTIVES], bool victory) {
    if (!filename) {
        return; }
    ofstream rep(filename);
    if (!rep.is_open()) {
        return; }
    rep << setfill('*') << setw(52) << "" << endl;
    rep << setfill(' ')<< setw(35) << "SESSION AUDIT LOG" << endl;
    rep << "Mission Outcome : " << (victory ? "VICTORY" : "DEFEAT") << endl;
    rep << "Total Score : " << score << " / 520" << endl;
    rep << "Energy Balance : " << remainingEnergy << " / " << MAX_ENERGY << endl;
    rep << "Time Remaining : " << remainingTime << endl;
    rep << "Objectives Audit:" << endl;
    rep << "Master Key (<) : " << (objectives[OBJ_MASTER_KEY] ? "DONE (+10)"  : "MISS") << endl;
    rep << "Clue 4 (F3-R2) : " << (objectives[OBJ_CLUE_4] ? "DONE (+10)" : "MISS") << endl;
    rep << "Clue 2 (F4-R3) : " << (objectives[OBJ_CLUE_2] ? "DONE (+10)" : "MISS") << endl;
    rep << "Clue 7 (F5-R1) : " << (objectives[OBJ_CLUE_7] ? "DONE (+10)" : "MISS") << endl;
    rep << "Cabinet Unlocked (427) : " << (objectives[OBJ_CABINET_UNLOCKED] ? "DONE (+30)" : "MISS") << endl;
    rep << "Train Key Acquired (^) : " << (objectives[OBJ_TRAIN_KEY] ? "DONE (+10)" : "MISS") << endl;
    rep << "Fuse Picked Up (*) : " << (objectives[OBJ_FUSE_COLLECTED] ? "DONE" : "MISS") << endl;
    rep << "Fuse Installed (F3-R4) : " << (objectives[OBJ_FUSE_INSTALLED] ? "DONE (+20)" : "MISS") << endl;
    rep << "Lift Pass Collected (() : " << (objectives[OBJ_LIFT_PASS] ? "DONE (+20)" : "MISS") << endl;
    rep << "Lift 2 Operational : " << (objectives[OBJ_LIFT2_ACTIVE] ? "DONE" : "MISS") << endl;
    rep << "Passenger Rescued (!) : " << (objectives[OBJ_PASSENGER_RESCUED] ? "DONE (+100)" : "MISS") << endl;
    rep << "Cat Carrier Picked Up (@): " << (objectives[OBJ_CAT_CARRIER] ? "DONE" : "MISS") << endl;
    rep << "Cat Secured (~) : " << (objectives[OBJ_CAT_SECURED] ? "DONE (+100)" : "MISS") << endl;
    rep << "Train Prepared (F5-R2) : " << (objectives[OBJ_TRAIN_PREPARED] ? "DONE (+100)" : "MISS") << endl;
    rep << "Full Mission Victory : " << (objectives[OBJ_MISSION_WON] ? "DONE (+100)" : "MISS") << endl;
    rep << setfill('*') << setw(52) << "" << endl;
    rep.close();
}

        




