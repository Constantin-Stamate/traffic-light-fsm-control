#ifndef LED_H
#define LED_H

#include <Arduino.h>
#include "../fsm/fsm.h"

void ledInit();
void setNightLights(bool yellowOn);

void updateLights(
    const TrafficLightFSM& eastWest,
    const TrafficLightFSM& northSouth
);

#endif