#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

void buttonInit(uint8_t pin);
void buttonSetRequestSemaphore(SemaphoreHandle_t semaphore);
void buttonTask(void *pvParameters);

#endif