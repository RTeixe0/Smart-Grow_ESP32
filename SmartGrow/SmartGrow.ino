#include <Arduino.h>

#include "secrets.h"
#include "config.h"

#include "include/Sensors.h"
#include "include/Actuators.h"
#include "include/TimeManager.h"
#include "include/WebServerManager.h"

extern bool isCoolerAuto;

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

  // Lógica da luz
  bool lightShouldBeOn = false;
  if (horaLigacao < horaDesligamento) {
    lightShouldBeOn = (currentHour > horaLigacao || 
                      (currentHour == horaLigacao && currentMinute >= minutoLigacao)) &&
                      (currentHour < horaDesligamento || 
                      (currentHour == horaDesligamento && currentMinute < minutoDesligamento));
  } else {
    lightShouldBeOn = (currentHour > horaLigacao || 
                      (currentHour == horaLigacao && currentMinute >= minutoLigacao)) ||
                      (currentHour < horaDesligamento || 
                      (currentHour == horaDesligamento && currentMinute < minutoDesligamento));
  }

  if (isLightAuto) {
    lightShouldBeOn ? turnLightOn() : turnLightOff();
  }

  // Lógica dos coolers
  static unsigned long lastCoolerAction = 0;
  static bool coolerState = false;

  float temperatura = readTemperature();
  unsigned long agora = millis();

  if (isCoolerAuto) {
    if (temperatura > 28.0) {
      if (!coolerState) {
        turnCoolersOn();
        coolerState = true;
        lastCoolerAction = agora;
      } else if (agora - lastCoolerAction >= 15 * 60 * 1000) {
        turnCoolersOff();
        coolerState = false;
        lastCoolerAction = agora;
      }
    } else if (isLightOn()) {
      if (agora - lastCoolerAction >= 15 * 60 * 1000) {
        coolerState = !coolerState;
        lastCoolerAction = agora;
        coolerState ? turnCoolersOn() : turnCoolersOff();
      }
    } else {
      unsigned long intervalo = coolerState ? 5 * 60 * 1000 : 25 * 60 * 1000;
      if (agora - lastCoolerAction >= intervalo) {
        coolerState = !coolerState;
        lastCoolerAction = agora;
        coolerState ? turnCoolersOn() : turnCoolersOff();
      }
    }
      }
}
