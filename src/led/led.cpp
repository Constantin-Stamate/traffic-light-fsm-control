#include "led.h"

#define EST_RED 30
#define EST_YELLOW 31
#define EST_GREEN 32

#define NORD_RED 33
#define NORD_YELLOW 34
#define NORD_GREEN 35

void ledInit() {
    pinMode(EST_RED, OUTPUT);
    pinMode(EST_YELLOW, OUTPUT);
    pinMode(EST_GREEN, OUTPUT);

    pinMode(NORD_RED, OUTPUT);
    pinMode(NORD_YELLOW, OUTPUT);
    pinMode(NORD_GREEN, OUTPUT);

    digitalWrite(EST_RED, HIGH);
    digitalWrite(EST_YELLOW, LOW);
    digitalWrite(EST_GREEN, LOW);

    digitalWrite(NORD_RED, HIGH);
    digitalWrite(NORD_YELLOW, LOW);
    digitalWrite(NORD_GREEN, LOW);
}

void setNightLights(bool yellowOn) {
    digitalWrite(EST_RED, LOW);
    digitalWrite(EST_GREEN, LOW);

    digitalWrite(NORD_RED, LOW);
    digitalWrite(NORD_GREEN, LOW);

    digitalWrite(EST_YELLOW, yellowOn ? HIGH : LOW);
    digitalWrite(NORD_YELLOW, yellowOn ? HIGH : LOW);
}

void updateLights(
    const TrafficLightFSM& eastWest,
    const TrafficLightFSM& northSouth
) {
    digitalWrite(EST_RED, LOW);
    digitalWrite(EST_YELLOW, LOW);
    digitalWrite(EST_GREEN, LOW);

    digitalWrite(NORD_RED, LOW);
    digitalWrite(NORD_YELLOW, LOW);
    digitalWrite(NORD_GREEN, LOW);

    switch (eastWest.getState()) {
        case TRAFFIC_RED:
            digitalWrite(EST_RED, HIGH);
            break;

        case TRAFFIC_YELLOW:
            digitalWrite(EST_YELLOW, HIGH);
            break;

        case TRAFFIC_GREEN:
            digitalWrite(EST_GREEN, HIGH);
            break;
    }

    switch (northSouth.getState()) {
        case TRAFFIC_RED:
            digitalWrite(NORD_RED, HIGH);
            break;

        case TRAFFIC_YELLOW:
            digitalWrite(NORD_YELLOW, HIGH);
            break;

        case TRAFFIC_GREEN:
            digitalWrite(NORD_GREEN, HIGH);
            break;
    }
}