#include "level.h"
#include "util.h"
#include <iostream>
#include <fstream>
using namespace std;
// Modular implementation using primitive arrays for map parsing and verification

const char symbols[16]= {
    ENT_MASTER_KEY, ENT_ENERGY_PACK, ENT_EMERGENCY_TOOLKIT, ENT_CLUE_4, ENT_FUSE, ENT_CAT, ENT_CONTROL_PANEL, ENT_LIFT_PASS,
    ENT_PASSENGER, ENT_MEDICAL_KIT, ENT_EMERGENCY_CARD, ENT_CLUE_2, ENT_CLUE_7, ENT_CABINET_TRAIN_KEY, ENT_CAT_CARRIER, ENT_OBSERVATION_TOKEN};

const char* names[16]= {
    "Master key", "Energy pack", "Emergency Toolkit", "Clue digit 4", "Fuse", "Cat", "Control panel", "Lift pass", "Injured Passenger",
    "Medical kit", "Emergency access card", "Clue digit 2", "Clue digit 7", "Security Cabinet/Train key", "Cat carrier", "Observation token"};

const char* getEntityName(char entitySymbol){
    int i = 0;
    while (i < 16){
        if (symbols[i]== entitySymbol){
            return names[i];
        } 
        i++;
    }
    return "Invalid entity.";
}

bool loadBuildingMap(const char* filename,
                     char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1],
                     char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                     int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR]){
                     
                        if(!filename) return 0;
                        
                        ifstream map(filename);
                        if(!map.is_open()) return 0;

                        for(int i = 0; i < NUM_FLOORS; i++){
                            for(int j = 0; j < ROOMS_PER_FLOOR; j++){
                                horizontalConn[i][j]= CONN_NONE;
                            }
                            for(int k = 0; k < ROOMS_PER_FLOOR; k++){
                                entityCounts[i][k] = 0;
                                for(int l = 0; l < MAX_ENTITIES_PER_ROOM; l++){
                                    roomEntities[i][k][l] = ENT_NONE;
                                }
                            }
                        }
                     }