#pragma once

typedef struct
{
    uint32_t watermarkValue;
    float batteryLevel;
    float temperature;
    float humidity;
} SensorValues_t;

typedef enum
{
    SET_SPEED = 0,
    WATERMARK_CH01_LEVEL = 1,
    BATTERY_LEVEL = 2
} SensorRegister;
