#include "serial_comm.h"

void serialInit() {
    Serial.begin(9600);

    Serial.println();
    Serial.println("Intelligent Traffic Light");
}

void printTrafficStates(
    const TrafficLightFSM& eastWest,
    const TrafficLightFSM& northSouth,
    const char* message
) {
    Serial.print(message);

    Serial.print(" | East-West: ");
    Serial.print(eastWest.getStateName());

    Serial.print(" | North-South: ");
    Serial.println(northSouth.getStateName());
}