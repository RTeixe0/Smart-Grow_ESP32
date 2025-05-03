#ifndef ACTUATORS_H
#define ACTUATORS_H

// Inicializa os pinos dos relés
void initActuators();

// Controle da luz
void turnLightOn();
void turnLightOff();

// Controle dos coolers
void turnCoolersOn();
void turnCoolersOff();

// Controle da bomba de água
void activatePump();

bool isLightOn();

#endif
