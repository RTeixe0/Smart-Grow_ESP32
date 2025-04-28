#include "include/TimeManager.h"
#include "config.h"

#include <Arduino.h>
#include <time.h>

void initTime() {
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
    
    Serial.println("Conectando ao servidor NTP...");
    delay(2000); // Pequena espera para sincronizar
    
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Falha ao obter hora do servidor NTP!");
    } else {
        Serial.println("Hora sincronizada com sucesso!");
    }
}

String getFormattedTime() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Hora não sincronizada! Tentando novamente...");
        initTime(); // Tenta ressincronizar
        delay(5000);
        if (!getLocalTime(&timeinfo)) {
            Serial.println("Erro ao obter hora do servidor NTP!");
            return "Erro ao obter hora";
        }
    }

    char buffer[30];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", &timeinfo);
    return String(buffer);
}

void getCurrentTime(int& hour, int& minute) {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Falha ao obter hora do servidor NTP!");
        hour = -1;
        minute = -1;
        return;
    }
    hour = timeinfo.tm_hour;
    minute = timeinfo.tm_min;
}
