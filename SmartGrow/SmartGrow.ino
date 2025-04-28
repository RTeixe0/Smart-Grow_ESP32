#include <Arduino.h>

#include "secrets.h"
#include "config.h"

#include "include/Sensors.h"
#include "include/Actuators.h"
#include "include/TimeManager.h"
#include "include/WebServerManager.h"

void setup() {
  Serial.begin(115200);

  // Inicializações principais
  initSensors();
  initActuators();
  initWiFi();
  initTime();
  initWebServer();
}

void loop() {
  handleOTA();
  handleWebServer();

  int currentHour, currentMinute;
  getCurrentTime(currentHour, currentMinute);

  if (currentHour == -1) return; // Falha ao obter hora

  bool lightShouldBeOn = false;

  if (horaLigacao < horaDesligamento) {
    // Intervalo no mesmo dia
    lightShouldBeOn = (currentHour > horaLigacao || 
                      (currentHour == horaLigacao && currentMinute >= minutoLigacao)) &&
                      (currentHour < horaDesligamento || 
                      (currentHour == horaDesligamento && currentMinute < minutoDesligamento));
  } else {
    // Intervalo que passa da meia-noite
    lightShouldBeOn = (currentHour > horaLigacao || 
                      (currentHour == horaLigacao && currentMinute >= minutoLigacao)) ||
                      (currentHour < horaDesligamento || 
                      (currentHour == horaDesligamento && currentMinute < minutoDesligamento));
  }

  if (isLightAuto) {
    if (lightShouldBeOn) {
      turnLightOn();
    } else {
      turnLightOff();
    }
  }
}
