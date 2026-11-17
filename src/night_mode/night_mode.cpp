#include "night_mode.h"

static uint8_t modePin;
static SemaphoreHandle_t modeSemaphore = nullptr;

void nightModeInit(uint8_t pin) {
    modePin = pin;
    pinMode(modePin, INPUT_PULLUP);
}

void nightModeSetSemaphore(SemaphoreHandle_t semaphore) {
    modeSemaphore = semaphore;
}

void nightModeTask(void* pvParameters) {
    (void)pvParameters;

    int lastReading = digitalRead(modePin);
    int stableState = lastReading;

    TickType_t lastChange = xTaskGetTickCount();
    TickType_t lastWake = xTaskGetTickCount();

    const TickType_t debounceTime = pdMS_TO_TICKS(50);
    const TickType_t period = 1;  

    for (;;) {
        int reading = digitalRead(modePin);
        TickType_t now = xTaskGetTickCount();

        if (reading != lastReading) {
            lastReading = reading;
            lastChange = now;
        }

        if ((now - lastChange) >= debounceTime &&
            reading != stableState) {

            stableState = reading;

            if (stableState == LOW &&
                modeSemaphore != nullptr) {

                xSemaphoreGive(modeSemaphore);
            }
        }

        vTaskDelayUntil(&lastWake, period);
    }
}