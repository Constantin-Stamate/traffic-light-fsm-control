#include "fsm.h"

TrafficLightFSM::TrafficLightFSM() {
    currentState = TRAFFIC_RED;
}

void TrafficLightFSM::init(TrafficState initialState) {
    currentState = initialState;
}

bool TrafficLightFSM::transitionTo(TrafficState newState) {
    bool valid = false;

    if (currentState == TRAFFIC_RED &&
        (newState == TRAFFIC_GREEN ||
         newState == TRAFFIC_YELLOW)) {
        valid = true;
    }

    if (currentState == TRAFFIC_GREEN &&
        (newState == TRAFFIC_YELLOW ||
         newState == TRAFFIC_RED)) {
        valid = true;
    }

    if (currentState == TRAFFIC_YELLOW &&
        newState == TRAFFIC_RED) {
        valid = true;
    }

    if (valid) {
        currentState = newState;
    }

    return valid;
}

TrafficState TrafficLightFSM::getState() const {
    return currentState;
}

const char* TrafficLightFSM::getStateName() const {
    switch (currentState) {
        case TRAFFIC_RED: return "RED";
        case TRAFFIC_YELLOW: return "YELLOW";
        case TRAFFIC_GREEN: return "GREEN";
        default: return "UNKNOWN";
    }
}