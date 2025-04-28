#include "include/Actuators.h"
#include "config.h"

#include <Arduino.h>

void initActuators() {
    pinMode(RELAY_PIN_LIGHT, OUTPUT);
    pinMode(RELAY_PIN_COOLERS, OUTPUT);
    pinMode(RELAY_PIN_PUMP, OUTPUT);

    // Inicializa os relés como desligados (HIGH)
    digitalWrite(RELAY_PIN_LIGHT, HIGH);
    digitalWrite(RELAY_PIN_COOLERS, HIGH);
    digitalWrite(RELAY_PIN_PUMP, HIGH);
}

// Controle da Luz
void turnLightOn() {
    digitalWrite(RELAY_PIN_LIGHT, LOW); 
}

void turnLightOff() {
    digitalWrite(RELAY_PIN_LIGHT, HIGH);
}

// Controle dos Coolers
void turnCoolersOn() {
    digitalWrite(RELAY_PIN_COOLERS, LOW);  
}

void turnCoolersOff() {
    digitalWrite(RELAY_PIN_COOLERS, HIGH); 
}

// Controle da Bomba
void activatePump() {
    digitalWrite(RELAY_PIN_PUMP, LOW);  
    delay(1500);                        
    digitalWrite(RELAY_PIN_PUMP, HIGH); 
}
