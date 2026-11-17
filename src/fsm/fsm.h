#ifndef FSM_H
#define FSM_H

#include <Arduino.h>

enum TrafficState {
    TRAFFIC_RED,
    TRAFFIC_YELLOW,
    TRAFFIC_GREEN
};

class TrafficLightFSM {
private:
    TrafficState currentState;

public:
    TrafficLightFSM();

    void init(TrafficState initialState);
    bool transitionTo(TrafficState newState);
    TrafficState getState() const;
    const char* getStateName() const;
};

#endif