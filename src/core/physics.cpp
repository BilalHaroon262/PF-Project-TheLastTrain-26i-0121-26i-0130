#include "physics.h"
#include "util.h"
#include "switches.h"

// Movement validation and resource adjustments without structures
bool isDirectlyConnected(int fromF, int fromR, int toF, int toR,
                         const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1],
                         char& outConnType) {
outConnType=CONN_NONE;
if (fromF < 1 || fromF > NUM_FLOORS || toF < 1 || toF > NUM_FLOORS) {
    return false; }
if (fromR < 1 || fromR > ROOMS_PER_FLOOR || toR < 1 || toR > ROOMS_PER_FLOOR) {
    return false; }
if (fromF == toF && fromR == toR) {
    return false; }
if (fromF == toF) {
    int step = toR - fromR;
    if (step != 1 && step != -1) {
    return false; }
    int linkIdx = (fromR < toR) ? (fromR - 1) : (toR - 1);
    char edge = horizontalConn[fromF - 1][linkIdx];
    outConnType = edge;
    switch (edge) {
    case CONN_DOOR:
    case CONN_WINDOW:
    case CONN_LOCKED_DOOR:
        return true;
    case '1':
        outConnType = CONN_DOOR;
        return true;
    default:
        return false; }
    }
//staircases S1 (1 to 2),S2 (2 to 3),S3 (4 to 5) at room1
if (fromR == 1 && toR == 1) { 
    int lowF = (fromF < toF) ? fromF : toF;
    int highF = (fromF > toF) ? fromF : toF;
    if ((lowF == 1 && highF == 2) || (lowF == 2 && highF == 3) ||(lowF == 4 && highF == 5)) {
        outConnType = CONN_DOOR;
        return true; } 
    }
//lift 1 between floor 1 and 2 (room 2 and 3)
if ((fromR == 2 || fromR == 3) && (toR == 2 || toR == 3)) {
    if (canUseLift1(fromF, toF)) {
        outConnType = '1';
        return true; }
    }
//lift 2 between floors 2, 4, and 5 at room 4
if (fromR == 4 && toR == 4) {
    bool currOnLift2 = (fromF == 2 || fromF == 4 || fromF == 5); //checking if current floor is 2,4 or 5
    bool desOnLift2 = (toF == 2 || toF == 4 || toF == 5); //checking if destination floor is 2,4 or 5
    if (currOnLift2 && desOnLift2 && fromF != toF) {
        outConnType = '2';
        return true; }
    }
    return false;
}
bool canTraverse(int targetFloor, int targetRoom, char connType, const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount,
                 int emergencyTripsLeft, bool lift2Operational) {
                    if (targetFloor < 1 || targetFloor > NUM_FLOORS || targetRoom < 1 || targetRoom > ROOMS_PER_FLOOR) {
                        return false; }
                    switch (connType) {
                        case CONN_DOOR:
                        case CONN_WINDOW:
                        case '1':
                            return true;
                        case CONN_LOCKED_DOOR:
                            return unlockFloor3Door(inventory,inventoryCount);
                        case '2': {
                            bool pass = lift2Operational && hasItem(inventory,inventoryCount,ENT_LIFT_PASS);
                            bool backup  = (emergencyTripsLeft > 0) && hasItem(inventory,inventoryCount,ENT_EMERGENCY_CARD);
                            return (pass || backup); }
                        default:
                            return false; }
                        }
void applyResourceDelta(int& energy, int& time, int deltaEnergy, int deltaTime) {
    energy = clampInt(energy+deltaEnergy,0,MAX_ENERGY);
    time = clampInt(time+deltaTime,0,MAX_TIME);
}
void applyScoreDelta(int& score, int deltaScore) {
    int updated = score + deltaScore;
    score = (updated < 0) ? 0 : updated;
}
bool hasItem(const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount, char itemSymbol) {
    if (inventoryCount < 0 || inventoryCount > MAX_INVENTORY_CAPACITY) {
        return false; }
    for (int i = 0; i < inventoryCount; i++) {
    if (inventory[i] == itemSymbol)
        return true;
    }
        return false;
}
bool addItem(char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount, char itemSymbol) {
    if (inventoryCount < 0 || inventoryCount >= MAX_INVENTORY_CAPACITY) {
    return false; }
    *(inventory + inventoryCount) = itemSymbol;
    inventoryCount += 1;
    return true;
}
bool removeItem(char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount, char itemSymbol) {
    if (inventoryCount < 0 || inventoryCount > MAX_INVENTORY_CAPACITY) {
    return false; }
    int foundPos = -1;
    for (int i = 0; i < inventoryCount; i++) {
       if (inventory[i] == itemSymbol) {
              foundPos = i;
                break; }
       }
    if (foundPos == -1) {
        return false; }
    for (int k = foundPos; k < inventoryCount - 1; k++) {
          inventory[k] = inventory[k + 1]; }
    inventory[inventoryCount - 1] = ENT_NONE;
    inventoryCount = inventoryCount - 1;
    return true;
}