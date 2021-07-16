#include <Arduino_EdgeControl.h>
#include <Wire.h>
#include "mbed.h"
#include "rtos.h"
#include <stdio.h>

#include "EdgeDisplay.hpp"
#include "SensorValues.hpp"
#include <water-prediction_inferencing.h>

// This board's I2C device
#define EDGE_I2C_ADDR 0x05

// Update every minute by default
uint16_t updatePeriod = 60000;

SensorValues_t vals;
uint8_t reg = 0;

static float buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = { 0 };

EdgeDisplay disp(&LCD);

void setup()
{
    initBoard();
}

void loop()
{
    updateSensors();
    delay(updatePeriod);
}

void initBoard()
{   
    EdgeControl.begin();

    Serial.begin(115200);
    delay(3000);

    Serial.println("Init begin");

    Power.on(PWR_3V3);
    Power.on(PWR_VBAT);
    Power.on(PWR_MKR1);

    Wire.begin();

    disp.begin(16, 2);

    Serial.print("IO Expander initializazion ");
    if (!Expander.begin()) {
        Serial.println("failed.");
        Serial.println("Please, be sure to enable gated 3V3 and 5V power rails");
        Serial.println("via Power.enable3V3() and Power.enable5V().");
    }
    else Serial.println("succeeded.");

    Watermark.begin();
    Watermark.calibrationMode(OUTPUT);
    Watermark.calibrationWrite(LOW);
    Watermark.commonMode(OUTPUT);
    Watermark.commonWrite(HIGH);
    Watermark.enable();

    Serial.println("Init end");
}

void updateSensors()
{   
    getDHTValues();
    vals.watermarkValue = Watermark.analogRead(WATERMARK_CH01);
    vals.batteryLevel = Power.getVBat();

    buffer[0] = (float)(vals.watermarkValue);
    buffer[1] = vals.batteryLevel;
    buffer[2] = vals.temperature;
    buffer[3] = vals.humidity;

    signal_t signal;
    int err = numpy::signal_from_buffer(buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);

    ei_impulse_result_t result = { 0 };
    err = run_classifier(&signal, &result);
    
    ei_printf("Predictions ");
    ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)",
        result.timing.dsp, result.timing.classification, result.timing.anomaly);
    ei_printf(": \n");
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        ei_printf("    %s: %.5f\n", result.classification[ix].label, result.classification[ix].value);
    }

    const char *resultLabel = getMax(result.classification);
    Serial.println(resultLabel);

    sendValues(&vals);
    logValues(&vals);
}

const char* getMax(ei_impulse_result_classification_t *classifications)
{
    uint8_t maxLabelIndex = 0;

    if(EI_CLASSIFIER_LABEL_COUNT > 1)
        for(uint8_t i = 1; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
            if(classifications[i].value > classifications[i - 1].value)
                maxLabelIndex = i;
        }
    
    return classifications[maxLabelIndex].label;
}

void logValues(SensorValues_t *values)
{
    char telem_buf[50];
    sprintf(telem_buf, "WTRMK: %lu, BATT: %2.4f, TEMP: %2.4f, HUMI: %2.4f",
        values->watermarkValue, values->batteryLevel, values->temperature, values->humidity);
    disp.showTelemetry(String(telem_buf));
    Serial.println(telem_buf);
}

void getDHTValues()
{
    Wire.requestFrom(EDGE_I2C_ADDR, sizeof(SensorValues_t));
    
    uint8_t buf[40];
    for(uint8_t i = 0; i < sizeof(SensorValues_t); i++)
        if(Wire.available())
            buf[i] = Wire.read();
    
    SensorValues_t *values = (SensorValues_t*)buf;

    vals.temperature = values->temperature;
    vals.humidity = values->humidity;
}

void sendValues(SensorValues_t *values)
{
    writeBytes((uint8_t*)values, sizeof(SensorValues_t));
}

void writeBytes(uint8_t *buf, uint8_t len)
{
    Serial.println("Writing values");
    Wire.beginTransmission(EDGE_I2C_ADDR);

    for(uint8_t i = 0; i < len; i++) {
        Serial.print(buf[i]); Serial.print(" ");
        Wire.write(buf[i]);
    }

    Serial.println();

    Wire.endTransmission();
}

void ei_printf(const char *format, ...) {
   static char print_buf[1024] = { 0 };

   va_list args;
   va_start(args, format);
   int r = vsnprintf(print_buf, sizeof(print_buf), format, args);
   va_end(args);

   if (r > 0) {
       Serial.write(print_buf);
   }
}
