#include "include/Sensors.h"
#include "config.h"

#include <DHT.h>
#include <Arduino.h>

// Instância do sensor DHT
DHT dht(DHTPIN, DHTTYPE);

void initSensors() {
    dht.begin();
    pinMode(SOIL_MOISTURE_PIN, INPUT);
}

float readTemperature() {
    return dht.readTemperature();
}

float readHumidity() {
    return dht.readHumidity();
}

float readSoilMoisture() {
    int soilValue = analogRead(SOIL_MOISTURE_PIN);
    float soilPercent = map(soilValue, SOIL_SENSOR_WET, SOIL_SENSOR_DRY, 100, 0);
    return soilPercent;
}
