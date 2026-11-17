#ifndef NIGHT_MODE_H
#define NIGHT_MODE_H

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

void nightModeInit(uint8_t pin);
void nightModeSetSemaphore(SemaphoreHandle_t semaphore);
void nightModeTask(void *pvParameters);

#endif