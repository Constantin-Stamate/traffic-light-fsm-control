#include "button.h"

static uint8_t buttonPin;
static SemaphoreHandle_t requestSemaphore = nullptr;

static const TickType_t BUTTON_PERIOD = 1;
static const TickType_t DEBOUNCE_TIME = pdMS_TO_TICKS(50);

void buttonInit(uint8_t pin) {
    buttonPin = pin;
    pinMode(buttonPin, INPUT_PULLUP);
}

void buttonSetRequestSemaphore(SemaphoreHandle_t semaphore) {
    requestSemaphore = semaphore;
}

void buttonTask(void *pvParameters) {
    (void)pvParameters;

    int lastReading = digitalRead(buttonPin);
    int stableState = lastReading;

    TickType_t lastChange = xTaskGetTickCount();
    TickType_t lastWake = xTaskGetTickCount();

    for (;;) {
        int reading = digitalRead(buttonPin);
        TickType_t now = xTaskGetTickCount();

        if (reading != lastReading) {
            lastReading = reading;
            lastChange = now;
        }

        if ((now - lastChange) >= DEBOUNCE_TIME &&
            reading != stableState) {

            stableState = reading;

            if (stableState == LOW &&
                requestSemaphore != nullptr) {

                xSemaphoreGive(requestSemaphore);
            }
        }

        vTaskDelayUntil(&lastWake, BUTTON_PERIOD);
    }
}