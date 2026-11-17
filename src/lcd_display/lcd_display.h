#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <Arduino.h>
#include "../fsm/fsm.h"

void lcdInit();

void lcdShowStates(
    const TrafficLightFSM& eastWest,
    const TrafficLightFSM& northSouth,
    bool requestActive
);

void lcdShowNightMode();

#endif