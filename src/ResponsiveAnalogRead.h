#ifndef RESPONSIVE_ANALOG_READ_H
#define RESPONSIVE_ANALOG_READ_H

#include "pico/stdlib.h"
#include "hardware/adc.h"

typedef struct {
    int pin;
    int analogResolution;
    float snapMultiplier;
    bool sleepEnable;
    float activityThreshold;
    bool edgeSnapEnable;
    float smoothValue;
    unsigned long lastActivityMS;
    float errorEMA;
    bool sleeping;
    int rawValue;
    int responsiveValue;
    int prevResponsiveValue;
    bool responsiveValueHasChanged;
} ResponsiveAnalogRead;

void ResponsiveAnalogRead_begin(ResponsiveAnalogRead* this, int pin, bool sleepEnable, float snapMultiplier);
int ResponsiveAnalogRead_getValue(ResponsiveAnalogRead* this);
int ResponsiveAnalogRead_getRawValue(ResponsiveAnalogRead* this);
bool ResponsiveAnalogRead_hasChanged(ResponsiveAnalogRead* this);
bool ResponsiveAnalogRead_isSleeping(ResponsiveAnalogRead* this);
void ResponsiveAnalogRead_update(ResponsiveAnalogRead* this);
void ResponsiveAnalogRead_updateWithRawValue(ResponsiveAnalogRead* this, int rawValueRead);
void ResponsiveAnalogRead_setSnapMultiplier(ResponsiveAnalogRead* this, float newMultiplier);
void ResponsiveAnalogRead_enableSleep(ResponsiveAnalogRead* this);
void ResponsiveAnalogRead_disableSleep(ResponsiveAnalogRead* this);
void ResponsiveAnalogRead_enableEdgeSnap(ResponsiveAnalogRead* this);
void ResponsiveAnalogRead_disableEdgeSnap(ResponsiveAnalogRead* this);
void ResponsiveAnalogRead_setActivityThreshold(ResponsiveAnalogRead* this, float newThreshold);
void ResponsiveAnalogRead_setAnalogResolution(ResponsiveAnalogRead* this, int resolution);

#endif
