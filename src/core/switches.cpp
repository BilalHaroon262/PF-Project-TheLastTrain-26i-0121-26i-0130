#include "switches.h"
#include "util.h"
#include "physics.h"
// Door lock mechanics, lifts, and cabinet verification without structures
bool canUseLift1(int fromFloor, int toFloor) {
    if (fromFloor == 1 && toFloor == 2) {
        return true; }
    if (fromFloor == 2 && toFloor == 1) {
        return true; }
    return false;
}
bool canUseLift2(int fromFloor, int toFloor, bool lift2Operational, int emergencyTripsLeft) {
    if (fromFloor == toFloor) {
        return false; }
    bool currValid = (fromFloor == 2 || fromFloor == 4 || fromFloor == 5);
    bool desValid = (toFloor == 2 || toFloor == 4 || toFloor == 5);
    if (!currValid || !desValid) {
        return false; }
    if (lift2Operational) {
        return true; }
    return (emergencyTripsLeft > 0);
}
bool unlockFloor3Door(const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount) {
    return hasItem(inventory,inventoryCount,ENT_MASTER_KEY);
}
bool installFuseControlPanel(char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
                             bool objectives[NUM_OBJECTIVES], int& score) {
    if (objectives[OBJ_FUSE_INSTALLED]) {
        return false; }
    if (!hasItem(inventory,inventoryCount,ENT_FUSE)) {
        return false; }
    removeItem(inventory,inventoryCount,ENT_FUSE);
    objectives[OBJ_FUSE_INSTALLED] = true;
    objectives[OBJ_LIFT2_ACTIVE] = true;
    applyScoreDelta(score,SCORE_INSTALL_FUSE);
    return true;
}
bool verifyCabinetCode(const char* enteredCode, const bool objectives[NUM_OBJECTIVES]) {
    if (!enteredCode) {
        return false; }
    bool ready = objectives[OBJ_CLUE_4] && objectives[OBJ_CLUE_2] && objectives[OBJ_CLUE_7];
    if (!ready) {
        return false; }
    return stringEquals(enteredCode,"427");
}