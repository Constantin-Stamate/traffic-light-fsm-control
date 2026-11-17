#include "lcd_display.h"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

static LiquidCrystal_I2C lcd(0x27, 16, 2);

static void printRow(uint8_t row, const char* text) {
    char line[17];
    uint8_t i = 0;

    while (text[i] != '\0' && i < 16) {
        line[i] = text[i];
        i++;
    }

    while (i < 16) {
        line[i++] = ' ';
    }

    line[16] = '\0';

    lcd.setCursor(0, row);
    lcd.print(line);
}

void lcdInit() {
    lcd.init();
    lcd.backlight();

    printRow(0, "Traffic Light FSM");
    printRow(1, "Starting...");
}

void lcdShowStates(
    const TrafficLightFSM& eastWest,
    const TrafficLightFSM& northSouth,
    bool requestActive
) {
    char row0[17];
    char row1[17];

    snprintf(
        row0,
        sizeof(row0),
        "EW: %s",
        eastWest.getStateName()
    );

    snprintf(
        row1,
        sizeof(row1),
        "NS: %-6s%s",
        northSouth.getStateName(),
        requestActive ? " REQ" : ""
    );

    printRow(0, row0);
    printRow(1, row1);
}

void lcdShowNightMode() {
    printRow(0, "NIGHT MODE");
    printRow(1, "Flashing yellow");
}