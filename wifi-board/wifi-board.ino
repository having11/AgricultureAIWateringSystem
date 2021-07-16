#include <SPI.h>
#include <Wire.h>
#include <WiFiNINA.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ArduinoJson.h>

#define DHTPIN 2
#define DHTTYPE DHT22

#define SELF_I2C_ADDR 0x05

#include "arduino_secrets.h"
#include "SensorValues.hpp"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PSK;

int status = WL_IDLE_STATUS;

WiFiClient client;
DHT dht = DHT(DHTPIN, DHTTYPE);

// Change what's below to your host's local IP address
IPAddress server(0, 0, 0, 0);

void setup()
{
    Serial.begin(115200);

    if(WiFi.status() == WL_NO_MODULE)
        while(1);
    
    while(status != WL_CONNECTED) {
        status = WiFi.begin(ssid, pass);
        delay(10000);
    }

    dht.begin();

    Wire.begin(SELF_I2C_ADDR);

    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);
}

void loop()
{
    
}

void sendValues(SensorValues_t *vals)
{
    StaticJsonDocument<200> doc;
    doc["watermark"] = vals->watermarkValue;
    doc["battery"] = vals->batteryLevel;
    doc["temperature"] = vals->temperature;
    doc["humidity"] = vals->humidity;

    String output;
    serializeJson(doc, output);
    Serial.println(output);

    client.stop();

    if(client.connect(server, 8080)) {
        client.println("POST /gardenEdge HTTP/1.1");
        // Change to your host's local IP address
        client.println("HOST: <HOST IP HERE>");
        client.println("User-Agent: Arduino/MKR WiFi 1010");
        client.println("Connection: close");
        client.println("Content-Type: application/json");
        client.print("Content-Length: ");
        client.println(output.length());
        client.println();
        client.println(output);
    }
    
    client.stop();
}

void receiveEvent(int bytes)
{
    uint8_t buf[32];
    uint8_t *ptr = &buf[0];

    SensorValues_t *vals;

    Serial.println("Receive event");

    while(Wire.available() > 0) {
        *ptr = Wire.read();
        ptr++;
    }

    vals = (SensorValues_t*)buf;

    sendValues(vals);
}

void requestEvent()
{
    Serial.println("Request event");

    SensorValues_t vals;

    vals.temperature = dht.readTemperature();
    vals.humidity = dht.readHumidity();

    uint8_t *buf = (uint8_t*)(&vals);

    for(uint8_t i = 0; i < sizeof(SensorValues_t); i++)
        Wire.write(buf[i]);
}
