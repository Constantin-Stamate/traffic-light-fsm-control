#ifndef SERIAL_COMM_H
#define SERIAL_COMM_H

#include <Arduino.h>
#include "../fsm/fsm.h"

void serialInit();

void printTrafficStates(
    const TrafficLightFSM& eastWest,
    const TrafficLightFSM& northSouth,
    const char* message
);

#endif