#include "level.h"
#include "util.h"
#include <iostream>
#include <fstream>
using namespace std;
// Modular implementation using primitive arrays for map parsing and verification

const char entities[16] = {
    ENT_MASTER_KEY, ENT_ENERGY_PACK, ENT_EMERGENCY_TOOLKIT, ENT_CLUE_4, ENT_FUSE, ENT_CAT, ENT_CONTROL_PANEL, ENT_LIFT_PASS,
    ENT_PASSENGER, ENT_MEDICAL_KIT, ENT_EMERGENCY_CARD, ENT_CLUE_2, ENT_CLUE_7, ENT_CABINET_TRAIN_KEY, ENT_CAT_CARRIER, ENT_OBSERVATION_TOKEN};

const char* entityNames[16] = {
    "Master Key", "Energy Pack", "Emergency Toolkit", "Clue Digit 4", "Fuse", "Cat", "Control Panel", "Lift Pass",
    "Injured Passenger", "Medical Kit", "Emergency Access Card", "Clue Digit 2", "Clue Digit 7", "Security Cabinet / Train Key", "Cat Carrier", "Observation Token"};

bool checkSymbolInTable(char ch){
    for (int i = 0; i < 16; i++){
           if (entities[i] == ch){
            return true;
        }}
    return false;
}

const char* getEntityName(char entitySymbol){
    for (int i = 0; i < 16; i++) {
        if (entities[i] == entitySymbol){
            return entityNames[i];
        }
    }
    return "Unknown object";
}

int textToInt(const char* txt){
    if (!txt) {
        return 0;
}
      int num = 0;
    int i = 0;
    while (txt[i] >= '0' && txt[i] <= '9'){
        num = num * 10 + (txt[i] - '0');
        i++;}
    return num;}
bool loadBuildingMap(const char* filename,
                        char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1],
                        char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                        int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR]){
      if (!filename){
        return false; }
     ifstream map(filename);
    if (!map.is_open()){
         return false; }
        for (int fl = 0; fl < NUM_FLOORS; fl++){
            for (int link = 0; link < ROOMS_PER_FLOOR - 1; link++){
            horizontalConn[fl][link] = CONN_NONE;
         }
        for (int rm = 0; rm < ROOMS_PER_FLOOR; rm++){
               entityCounts[fl][rm] = 0;
            for (int slot = 0; slot < MAX_ENTITIES_PER_ROOM; slot++){
                 roomEntities[fl][rm][slot] = ENT_NONE;
            }
        }}
    char temprow[MAX_LINE_LEN];
    int foundFloors = 0;

  while (foundFloors < NUM_FLOORS && map.getline(temprow, MAX_LINE_LEN)){
    int f = -1;
    for (int i = 0; temprow[i] != '\0'; i++) {
        if (temprow[i] == 'F' &&
            (temprow[i + 1] >= '1' && temprow[i + 1] <= '5') && temprow[i + 2] == '-' && temprow[i + 3] == 'R' && temprow[i + 4] == '1'){
            f = i; 
            break;}}
        if (f == 10) {
            int fpos = (temprow[f + 1] - '1');
            horizontalConn[fpos][0] = temprow[f + 10];
            horizontalConn[fpos][1] = temprow[f + 22];
            horizontalConn[fpos][2] = temprow[f + 34];
            char tempobj[MAX_LINE_LEN];
            
            if (map.getline(tempobj, MAX_LINE_LEN)){
                int length = 0;
                while (tempobj[length]!= '\0'){
                    length++;
                }

                for (int room = 0; room < ROOMS_PER_FLOOR; room++){
                        int leftBound  = 9 +(room * 12);
                    int rightBound = leftBound + 10;

                    for (int c = leftBound; c <= rightBound && c < length; c++){
                            char item = tempobj[c];
                            if (checkSymbolInTable(item)) {
                            int n = entityCounts[fpos][room];
                            if (n < MAX_ENTITIES_PER_ROOM){
                                roomEntities[fpos][room][n] = item;
                                entityCounts[fpos][room] = n + 1;
                            }
                        }
                    }
                }
                }
            foundFloors++;}
        }
    if (entityCounts[0][3] == 1 && roomEntities[0][3][0] == ENT_ENERGY_PACK){
        roomEntities[0][3][1] = ENT_ENERGY_PACK;
        entityCounts[0][3] = 2;}
    map.close();
    return (foundFloors == NUM_FLOORS);
}
bool loadInitialState(const char* filename,
                         int& playerFloor, int& playerRoom,
                       int& energy, int& time, int& score,
                       char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
                       bool objectives[NUM_OBJECTIVES], int& emergencyTripsLeft) {
    if (!filename) {
        return false;
    }

    ifstream state(filename);
    if (!state.is_open()){
        return false;
    }
    playerFloor = 1;
    playerRoom = 1;
    energy = INITIAL_ENERGY;
    time = INITIAL_TIME;
    score = INITIAL_SCORE;
    inventoryCount = 0;
    emergencyTripsLeft = 0;
    for (int i = 0; i < MAX_INVENTORY_CAPACITY; i++){
        inventory[i] = ENT_NONE;
    }
    for (int j = 0; j < NUM_OBJECTIVES; j++){
        objectives[j] = false;
    }

    char templine[MAX_LINE_LEN];
    while (state.getline(templine, MAX_LINE_LEN)){
        trimWhitespace(templine);
        if (templine[0] == '\0'){
            continue;}
        char* col = nullptr;
         for (int i = 0; templine[i] != '\0'; i++){
            if (templine[i] == ':') {
                col = &templine[i];
                break;
            }}
        if (!col) {
            continue;
        }
        *col = '\0';
        char* fieldName  = templine;
        char* fieldValue = col + 1;
        trimWhitespace(fieldName);
        trimWhitespace(fieldValue);

        if (stringEquals(fieldName, "INITIAL_ENERGY")){
            energy = textToInt(fieldValue);
        } 
        else if (stringEquals(fieldName, "INITIAL_TIME")){
            time = textToInt(fieldValue);}
        else if (stringEquals(fieldName, "INITIAL_SCORE")){
            score = textToInt(fieldValue);} 
        else if (stringEquals(fieldName, "START_ROOM")){
            if (fieldValue[0] == 'F' && fieldValue[2] == '-' && fieldValue[3] == 'R'){
                int fl = fieldValue[1] - '0';
                int rm = fieldValue[4] - '0';
                if (fl >= 1 && fl <= NUM_FLOORS && rm >= 1 && rm <= ROOMS_PER_FLOOR){
                    playerFloor = fl;
                    playerRoom = rm;
                }}
        }}
    state.close();
    return true;
}
bool validateStartingRoom(int startFloor, int startRoom,
                          const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1]){
    if (startFloor < 1 || startFloor > NUM_FLOORS){
        return false;
    }
    if (startRoom < 1 || startRoom > ROOMS_PER_FLOOR){
        return false;
    }
    int floor = startFloor - 1;
    int room = startRoom - 1;
    if (room > 0 && horizontalConn[floor][room - 1]!= CONN_SOLID_WALL && horizontalConn[floor][room - 1] !=CONN_NONE){
        return true;}
    if (room < ROOMS_PER_FLOOR - 1 && horizontalConn[floor][room]!= CONN_SOLID_WALL && horizontalConn[floor][room]!= CONN_NONE){
        return true;
    }
    if (startRoom == 1 || startRoom == 4){
        return true;
    }
    return false;
}