#include "ResponsiveAnalogRead.h"

void ResponsiveAnalogRead_begin(ResponsiveAnalogRead* this, int pin, bool sleepEnable, float snapMultiplier) {
    this->pin = pin;
    this->sleepEnable = sleepEnable;
    this->snapMultiplier = snapMultiplier;
    this->analogResolution = 1024;
    this->activityThreshold = 4.0;
    this->edgeSnapEnable = true;
    this->lastActivityMS = 0;
    this->errorEMA = 0.0;
    this->sleeping = false;
    this->rawValue = 0;
    this->responsiveValue = 0;
    this->prevResponsiveValue = 0;
    this->responsiveValueHasChanged = false;
}

int ResponsiveAnalogRead_getValue(ResponsiveAnalogRead* this) {
    return this->responsiveValue;
}

int ResponsiveAnalogRead_getRawValue(ResponsiveAnalogRead* this) {
    return this->rawValue;
}

bool ResponsiveAnalogRead_hasChanged(ResponsiveAnalogRead* this) {
    return this->responsiveValueHasChanged;
}

bool ResponsiveAnalogRead_isSleeping(ResponsiveAnalogRead* this) {
    return this->sleeping;
}

void ResponsiveAnalogRead_update(ResponsiveAnalogRead* this) {
    this->rawValue = adc_read();
    ResponsiveAnalogRead_updateWithRawValue(this, this->rawValue);
}

void ResponsiveAnalogRead_updateWithRawValue(ResponsiveAnalogRead* this, int rawValueRead) {
    this->rawValue = rawValueRead;
    this->prevResponsiveValue = this->responsiveValue;
    this->responsiveValue = ResponsiveAnalogRead_getResponsiveValue(this, this->rawValue);
    this->responsiveValueHasChanged = this->responsiveValue != this->prevResponsiveValue;
}

void ResponsiveAnalogRead_setSnapMultiplier(ResponsiveAnalogRead* this, float newMultiplier) {
    if (newMultiplier > 1.0) {
        newMultiplier = 1.0;
    }
    if (newMultiplier < 0.0) {
        newMultiplier = 0.0;
    }
    this->snapMultiplier = newMultiplier;
}

void ResponsiveAnalogRead_enableSleep(ResponsiveAnalogRead* this) {
    this->sleepEnable = true;
}

void ResponsiveAnalogRead_disableSleep(ResponsiveAnalogRead* this) {
    this->sleepEnable = false;
}

void ResponsiveAnalogRead_enableEdgeSnap(ResponsiveAnalogRead* this) {
    this->edgeSnapEnable = true;
}

void ResponsiveAnalogRead_disableEdgeSnap(ResponsiveAnalogRead* this) {
    this->edgeSnapEnable = false;
}

void ResponsiveAnalogRead_setActivityThreshold(ResponsiveAnalogRead* this, float newThreshold) {
    this->activityThreshold = newThreshold;
}

void ResponsiveAnalogRead_setAnalogResolution(ResponsiveAnalogRead* this, int resolution) {
    this->analogResolution = resolution;
}

int ResponsiveAnalogRead_getResponsiveValue(ResponsiveAnalogRead* this, int newValue) {
    if (this->sleepEnable && this->edgeSnapEnable) {
        if (newValue < this->activityThreshold) {
            newValue = (newValue * 2) - this->activityThreshold;
        } else if (newValue > this->analogResolution - this->activityThreshold) {
            newValue = (newValue * 2) - this->analogResolution + this->activityThreshold;
        }
    }
    unsigned int diff = abs(newValue - this->smoothValue);
    this->errorEMA += ((newValue - this->smoothValue) - this->errorEMA) * 0.4;
    if (this->sleepEnable) {
        this->sleeping = abs(this->errorEMA) < this->activityThreshold;
    }
    if (this->sleepEnable && this->sleeping) {
        return (int)this->smoothValue;
    }
    float snap = ResponsiveAnalogRead_snapCurve(this, diff * this->snapMultiplier);
    if (this->sleepEnable) {
        snap *= 0.5 + 0.5;
    }
    this->smoothValue += (newValue - this->smoothValue) * snap;
    if (this->smoothValue < 0.0) {
        this->smoothValue = 0.0;
    } else if (this->smoothValue > this->analogResolution - 1) {
        this->smoothValue = this->analogResolution - 1;
    }
    return (int)this->smoothValue;
}

float ResponsiveAnalogRead_snapCurve(ResponsiveAnalogRead* this, float x) {
    float y = 1.0 / (x + 1.0);
    y = (1.0 - y) * 2.0;
    if (y > 1.0) {
        return 1.0;
    }
    return y;
}
