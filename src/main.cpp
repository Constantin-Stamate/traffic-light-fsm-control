#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "button/button.h"
#include "fsm/fsm.h"
#include "led/led.h"
#include "serial_comm/serial_comm.h"
#include "night_mode/night_mode.h"
#include "lcd_display/lcd_display.h"

#define BUTTON_NORD 22
#define BUTTON_NIGHT 23

#define GREEN_TIME pdMS_TO_TICKS(5000)
#define YELLOW_TIME pdMS_TO_TICKS(2000)
#define ALL_RED_TIME pdMS_TO_TICKS(500)
#define BLINK_TIME pdMS_TO_TICKS(500)

static TrafficLightFSM eastWestFSM;
static TrafficLightFSM northSouthFSM;

static SemaphoreHandle_t northRequestSemaphore = nullptr;
static SemaphoreHandle_t nightSemaphore = nullptr;

static bool nightMode = false;
static bool requestActive = false;

static void applyStates(
    TrafficState eastWest,
    TrafficState northSouth,
    const char* message
) {
    if (eastWestFSM.getState() != eastWest) {
        if (!eastWestFSM.transitionTo(eastWest)) {
            Serial.println(F("Invalid East-West transition"));
            return;
        }
    }

    if (northSouthFSM.getState() != northSouth) {
        if (!northSouthFSM.transitionTo(northSouth)) {
            Serial.println(F("Invalid North-South transition"));
            return;
        }
    }

    updateLights(eastWestFSM, northSouthFSM);

    printTrafficStates(
        eastWestFSM,
        northSouthFSM,
        message
    );

    lcdShowStates(
        eastWestFSM,
        northSouthFSM,
        requestActive
    );
}

static void initializeIntersection(const char* message) {
    eastWestFSM.init(TRAFFIC_GREEN);
    northSouthFSM.init(TRAFFIC_RED);

    requestActive = false;

    updateLights(eastWestFSM, northSouthFSM);

    printTrafficStates(
        eastWestFSM,
        northSouthFSM,
        message
    );

    lcdShowStates(
        eastWestFSM,
        northSouthFSM,
        requestActive
    );
}

static bool processNightRequest() {
    if (xSemaphoreTake(nightSemaphore, 0) != pdTRUE) {
        return false;
    }

    nightMode = !nightMode;

    if (nightMode) {
        Serial.println(F("NIGHT MODE ON: Flashing yellow"));

        requestActive = false;

        xSemaphoreTake(northRequestSemaphore, 0);

        setNightLights(false);
        lcdShowNightMode();
    } else {
        Serial.println(F("NIGHT MODE OFF"));

        xSemaphoreTake(northRequestSemaphore, 0);

        initializeIntersection(
            "Normal mode: East-West GREEN"
        );
    }

    return true;
}

static bool waitOrNight(TickType_t duration) {
    const TickType_t interval = pdMS_TO_TICKS(10);
    const TickType_t start = xTaskGetTickCount();

    while (
        (TickType_t)(xTaskGetTickCount() - start) < duration
    ) {
        if (processNightRequest()) {
            return false;
        }

        vTaskDelay(interval);
    }

    return true;
}

static bool runNorthCycle() {
    Serial.println(F("North-South request received"));

    requestActive = true;

    applyStates(
        TRAFFIC_YELLOW,
        TRAFFIC_RED,
        "East-West YELLOW"
    );

    if (!waitOrNight(YELLOW_TIME)) {
        return false;
    }

    applyStates(
        TRAFFIC_RED,
        TRAFFIC_RED,
        "All directions RED"
    );

    if (!waitOrNight(ALL_RED_TIME)) {
        return false;
    }

    applyStates(
        TRAFFIC_RED,
        TRAFFIC_GREEN,
        "North-South GREEN"
    );

    if (!waitOrNight(GREEN_TIME)) {
        return false;
    }

    applyStates(
        TRAFFIC_RED,
        TRAFFIC_YELLOW,
        "North-South YELLOW"
    );

    if (!waitOrNight(YELLOW_TIME)) {
        return false;
    }

    applyStates(
        TRAFFIC_RED,
        TRAFFIC_RED,
        "All directions RED"
    );

    if (!waitOrNight(ALL_RED_TIME)) {
        return false;
    }

    requestActive = false;

    applyStates(
        TRAFFIC_GREEN,
        TRAFFIC_RED,
        "East-West GREEN"
    );

    Serial.println(F("North-South cycle completed"));

    return true;
}

static void trafficControllerTask(void* pvParameters) {
    (void)pvParameters;

    initializeIntersection(
        "System started: East-West GREEN"
    );

    bool yellowOn = false;
    TickType_t lastBlink = xTaskGetTickCount();

    for (;;) {
        if (processNightRequest()) {
            yellowOn = false;
            lastBlink = xTaskGetTickCount();
        }

        if (nightMode) {
            xSemaphoreTake(northRequestSemaphore, 0);

            TickType_t now = xTaskGetTickCount();

            if (
                (TickType_t)(now - lastBlink) >= BLINK_TIME
            ) {
                yellowOn = !yellowOn;

                setNightLights(yellowOn);

                lastBlink = now;
            }

            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        if (
            xSemaphoreTake(
                northRequestSemaphore,
                pdMS_TO_TICKS(10)
            ) == pdTRUE
        ) {
            if (processNightRequest()) {
                yellowOn = false;
                lastBlink = xTaskGetTickCount();
                continue;
            }

            if (nightMode) {
                continue;
            }

            if (!runNorthCycle()) {
                yellowOn = false;
                lastBlink = xTaskGetTickCount();

                continue;
            }
        }
    }
}

void setup() {
    serialInit();

    ledInit();
    lcdInit();

    buttonInit(BUTTON_NORD);
    nightModeInit(BUTTON_NIGHT);

    northRequestSemaphore = xSemaphoreCreateBinary();
    nightSemaphore = xSemaphoreCreateBinary();

    if (
        northRequestSemaphore == nullptr ||
        nightSemaphore == nullptr
    ) {
        Serial.println(F("Semaphore creation failed"));
        Serial.flush();

        for (;;) {
        }
    }

    buttonSetRequestSemaphore(northRequestSemaphore);
    nightModeSetSemaphore(nightSemaphore);

    BaseType_t buttonResult = xTaskCreate(
        buttonTask,
        "BtnNord",
        160,
        nullptr,
        2,
        nullptr
    );

    BaseType_t nightResult = xTaskCreate(
        nightModeTask,
        "BtnNight",
        160,
        nullptr,
        2,
        nullptr
    );

    BaseType_t trafficResult = xTaskCreate(
        trafficControllerTask,
        "Traffic",
        400,
        nullptr,
        1,
        nullptr
    );

    if (
        buttonResult != pdPASS ||
        nightResult != pdPASS ||
        trafficResult != pdPASS
    ) {
        Serial.println(F("FreeRTOS initialization failed"));
        Serial.flush();

        for (;;) {
        }
    }
}

void loop() {
}