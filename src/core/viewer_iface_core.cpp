#include "viewer_iface.h"
#include "level.h"
#include "util.h"
#include <iostream>
using namespace std;

const char* ROOM_TITLES[NUM_FLOORS][ROOMS_PER_FLOOR] = {
    {"Entrance Lobby", "Reception", "Waiting Area", "Utility Room"}, {"Lounge", "Archive", "Lab", "Meeting Room"},
    {"Stairwell Annex", "Security Vestibule", "Animal Room", "Control Room"}, {"First Aid Station","Medical Room", "Maintenance Corridor", "Office"},
    {"Security Room", "Train Boarding Platform", "Storage Room", "Observation Deck"}
};

const char* lookupRoomTitle(int fl, int rm) {
    if (fl >= 1 && fl <= NUM_FLOORS && rm >= 1 && rm <= ROOMS_PER_FLOOR) {
        return ROOM_TITLES[fl - 1][rm - 1];
    }
    return "Invalid Room";
}

void renderDashboard(int playerFloor, int playerRoom, int energy, int time, int score,
                     const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount,
                     int emergencyTripsLeft) {
    cout << "CURRENT ROOM : F" << playerFloor << "-R" << playerRoom << "[" << lookupRoomTitle(playerFloor, playerRoom) << "]" << endl;
    cout << "ENERGY : " << energy << "/" << MAX_ENERGY << "|  TIME LEFT : " << time << "|  SCORE : " << score << "/520" << endl;
    cout << "EMERGENCY LIFT-2 TRIPS : " << emergencyTripsLeft << endl;
    cout << "ITEMS (" << inventoryCount << "/" << MAX_INVENTORY_CAPACITY << ") : ";
    if (inventoryCount <= 0){
        cout << "None";
    } 
    else{
        for (int i = 0; i < inventoryCount && i < MAX_INVENTORY_CAPACITY; i++) {
            cout << "(" << inventory[i] << " = " << getEntityName(inventory[i]) << ") ";
}
}
  }

void renderRoomDetails(int floor, int room,
                       const char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                       int entityCount) {
    if (floor < 1 || floor > NUM_FLOORS || room < 1 || room > ROOMS_PER_FLOOR){
        return;
    }
    cout << ">> Inspecting F" << floor << "-R" << room
         << " [" << lookupRoomTitle(floor, room) << "] — Visible Objects: ";
    if (entityCount <= 0) {
        cout << "Empty" << endl;
        return;
    }
    cout << endl;
    for (int k = 0; k < entityCount && k < MAX_ENTITIES_PER_ROOM; k++){
        char ch = roomEntities[floor - 1][room - 1][k];
        cout << "   * [" << ch << "] " << getEntityName(ch) << endl;}
}
void renderAsciiBuilding(
    int playerFloor, int playerRoom,
    const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1],
    const char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
    const int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR]){
    cout << "\n        +-----------+-----------+-----------+-----------+          L2" << endl;
    for (int f = NUM_FLOORS; f >= 1; f--) {
        cout << "     |  |";
        for (int r = 1; r <= ROOMS_PER_FLOOR; r++) {
            bool here = (playerFloor == f && playerRoom == r);
            cout << " F" << f << "-R" << r << (here ? " <P> " : "     ");

            if (r < ROOMS_PER_FLOOR){
                cout << horizontalConn[f - 1][r - 1];
            } else{
                cout << "|";
            }
        }
        cout << endl;
        cout << "     |  |";
        for (int r = 1; r <= ROOMS_PER_FLOOR; r++) {
            int flIndex = f - 1;
            int rmIndex = r - 1;
            int count = entityCounts[flIndex][rmIndex];

            cout << " ";
            int usedChars = 0;

            if (count > MAX_ENTITIES_PER_ROOM) {
                count = MAX_ENTITIES_PER_ROOM;}
            if (count < 0) {
                count = 0;
            }
            for (int k = 0; k < count; k++){
                if (k > 0) {
                    cout << " ";
                    usedChars++;
                }
                cout << roomEntities[flIndex][rmIndex][k];
                usedChars++;
            }
            while (usedChars < 10){
                cout << " ";
                usedChars++;}
            cout << "|";
}
        if (f == 5)      cout << "          |\nS3---|  +-----------+-----------+-----------+-----------+          |" << endl;
        else if (f == 4) cout << "          L2\n        +-----------+-----------+-----------+-----------+          ." <<endl;
        else if (f == 3) cout << "          .\nS2---|  +-----------+-----------+-----------+-----------+          ." << endl;
        else if (f == 2) cout << "==========L2\nS1---|  +-----------+-----------+-----------+-----------+" << endl;
        else             cout << "\n     |  +-----------+-----------+-----------+-----------+" << endl; }}
void renderObjectivesChecklist(const bool objectives[NUM_OBJECTIVES]){
    printHeader("MISSION PROGRESS TRACKER");
    cout << "  " << (objectives[OBJ_MASTER_KEY]? "[DONE]" :"[FAILED]") << " Master Key collected (+10)" << endl;
    cout << "  " << (objectives[OBJ_CLUE_4] ? "[DONE]" :"[FAILED]") << " Clue Digit 4 found in F3-R2 (+10)" << endl;
    cout << "  " << (objectives[OBJ_CLUE_2] ? "[DONE]" :"[FAILED]") << " Clue Digit 2 found in F4-R3 (+10)" << endl;
    cout << "  " << (objectives[OBJ_CLUE_7] ? "[DONE]" :"[FAILED]") << " Clue Digit 7 found in F5-R1 (+10)" << endl;
    cout << "  " << (objectives[OBJ_CABINET_UNLOCKED]? "[DONE]" :"[FAILED]") << " Cabinet unlocked with 427 (+30)" << endl;
    cout << "  " << (objectives[OBJ_TRAIN_KEY] ? "[DONE]" :"[FAILED]") << " Train Key obtained (+10)"<< endl;
    cout << "  " << (objectives[OBJ_FUSE_COLLECTED]? "[DONE]" :"[FAILED]") << " Fuse picked Up" << endl;
    cout << "  " << (objectives[OBJ_FUSE_INSTALLED]? "[DONE]" :"[FAILED]") << " Fuse installed at Control Panel (+20)" << endl;
    cout << "  " << (objectives[OBJ_LIFT_PASS] ? "[DONE]" :"[FAILED]") << " Lift Pass collected (+20)" << endl;
    cout << "  " << (objectives[OBJ_LIFT2_ACTIVE] ? "[DONE]" :"[FAILED]") << " Lift 2 power restored" << endl;
    cout << "  " << (objectives[OBJ_PASSENGER_RESCUED] ? "[DONE]" :"[FAILED]") << " Injured passenger rescued (+100)" << endl;
    cout << "  " << (objectives[OBJ_CAT_CARRIER] ? "[DONE]" :"[FAILED]") << " Cat Carrier picked Up" << endl;
    cout << "  " << (objectives[OBJ_CAT_SECURED] ? "[DONE]" :"[FAILED]") << " Stranded Cat secured (+100)" << endl;
    cout << "  " << (objectives[OBJ_TRAIN_PREPARED] ? "[DONE]" :"[FAILED]") << " Final Train prepared and boarded (+100)" << endl;
    cout << "  " << (objectives[OBJ_MISSION_WON] ? "[DONE]" :"[FAILED]") << " Mission victory bonus (+100)" << endl;
}
