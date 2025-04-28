#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>


// Configurações de IP Fixo
#define LOCAL_IP      IPAddress(192, 168, 15, 200)
#define GATEWAY       IPAddress(192, 168, 15, 1)
#define SUBNET        IPAddress(255, 255, 255, 0)


// Configurações de NTP
#define NTP_SERVER         "216.239.35.12"
#define GMT_OFFSET_SEC     -10800  // Fuso horário Brasília (UTC-3)
#define DAYLIGHT_OFFSET_SEC 0


// Definição dos Pinos

// Sensores
#define DHTPIN             15
#define DHTTYPE            DHT22
#define SOIL_MOISTURE_PIN  32

// Relés
#define RELAY_PIN_LIGHT    5   // Relé da Lâmpada
#define RELAY_PIN_COOLERS  2   // Relé dos Coolers
#define RELAY_PIN_PUMP     4   // Relé da Bomba d'água


// Outros
// Valores do sensor de umidade do solo para calibrar
#define SOIL_SENSOR_WET    1100  // Valor lido quando o solo está molhado
#define SOIL_SENSOR_DRY    2700  // Valor lido quando o solo está seco

#endif
