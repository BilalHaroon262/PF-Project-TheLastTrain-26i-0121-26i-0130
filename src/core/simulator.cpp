#include "simulator.h"
#include "physics.h"
#include "switches.h"
#include "logger.h"
#include "viewer_iface.h"
#include "util.h"
#include "level.h"
#include <iostream>
using namespace std;
// Modular simulation loop using primitive variables
void splitCommandLine(const char* raw, char verb[MAX_NAME_LEN], char param1[MAX_NAME_LEN],char param2[MAX_NAME_LEN]) {
    for (int i = 0; i < MAX_NAME_LEN; i++) {
        verb[i] ='\0';
        param1[i] = '\0';
        param2[i] = '\0'; }
    int pos=0;
    while (raw[pos]==' ' || raw[pos]=='\t')
        pos++;
    int w=0;
    while (raw[pos] != '\0' && raw[pos] != ' ' && raw[pos] != '\t' && (w < (MAX_NAME_LEN - 1))) {
        char c = raw[pos];
        if (c >= 'A' && c <= 'Z') {
            c = c + 32; }
        verb[w]=c;
        w++;
        pos++; }
    while (raw[pos] == ' ' || raw[pos] == '\t') {
        pos++; }
    w = 0;
    while (raw[pos] != '\0' && raw[pos] != ' ' && raw[pos] != '\t' && (w < (MAX_NAME_LEN - 1))) {
        param1[w] = raw[pos];
        w++;
        pos++; }
    while (raw[pos] == ' ' || raw[pos] == '\t') {
        pos++; }
    w = 0;
    while (raw[pos] != '\0' && raw[pos] != ' ' && raw[pos] != '\t' && (w < (MAX_NAME_LEN - 1))) {
        param2[w] = raw[pos];
        w++;
        pos++; }
}
