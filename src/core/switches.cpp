#include "switches.h"
#include "util.h"
#include "physics.h"
// Door lock mechanics, lifts, and cabinet verification without structures
bool canUseLift1(int fromFloor, int toFloor) {
    if (fromFloor == 1 && toFloor == 2) {
        return true;
    }
    if (fromFloor == 2 && toFloor == 1) {
        return true;
    }
    return false;
}