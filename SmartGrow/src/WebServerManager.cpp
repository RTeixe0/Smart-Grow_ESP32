#include "include/WebServerManager.h"
#include "config.h"
#include "secrets.h"
#include "include/Sensors.h"
#include "include/Actuators.h"
#include "include/TimeManager.h"

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoOTA.h>

// Instância do servidor Web
WebServer server(80);

// Variáveis de controle
bool isAutomaticMode = false;
bool isLightAuto = true;
int horaLigacao = 18;
int minutoLigacao = 0;
int horaDesligamento = 12;
int minutoDesligamento = 00;

// Funções internas auxiliares
void handleRoot();
void handleData();
void handleTurnLightOn();
void handleTurnLightOff();
void handleTurnCoolersOn();
void handleTurnCoolersOff();
void handleActivatePump();
void handleToggleMode();
void handleSetSchedule();
void handleToggleLightMode();

// Implementações principais

void initWiFi() {
    if (!WiFi.config(LOCAL_IP, GATEWAY, SUBNET)) {
        Serial.println("Falha ao configurar IP estático!");
    }
    WiFi.begin(ssid, password);
    Serial.print("Conectando ao Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWi-Fi conectado!");
    Serial.print("IP do ESP32: ");
    Serial.println(WiFi.localIP());

    ArduinoOTA.begin();
}

void initWebServer() {
    server.on("/", handleRoot);
    server.on("/data", handleData);
    server.on("/turnLightOn", handleTurnLightOn);
    server.on("/turnLightOff", handleTurnLightOff);
    server.on("/turnCoolersOn", handleTurnCoolersOn);
    server.on("/turnCoolersOff", handleTurnCoolersOff);
    server.on("/activatePump", handleActivatePump);
    server.on("/toggleMode", handleToggleMode);
    server.on("/setSchedule", handleSetSchedule);
    server.on("/toggleLightMode", handleToggleLightMode);

    server.begin();
    Serial.println("Servidor Web iniciado");
}

void handleWebServer() {
    server.handleClient();
}

void handleOTA() {
    ArduinoOTA.handle();
}


// Implementações das rotas
void handleRoot() {
    String html = "<!DOCTYPE html><html lang='pt-BR'><head>"
                  "<meta charset='UTF-8'>"
                  "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
                  "<title>Smart Grow</title>"
                  "<style>"
                  "body { margin: 0; font-family: Arial, sans-serif; background: #f2f2f2; display: flex; flex-direction: column; align-items: center; padding: 20px; }"
                  "h2 { color: #333; }"
                  "#container, #scheduleContainer { background: #fff; width: 100%; max-width: 400px; margin: 10px 0; padding: 20px; border-radius: 10px; box-shadow: 0 2px 8px rgba(0,0,0,0.1); }"
                  "p { font-size: 18px; margin: 10px 0; }"
                  "span { font-weight: bold; color: #4CAF50; }"
                  "button { margin-top: 10px; width: 100%; padding: 10px; font-size: 16px; background-color: #4CAF50; color: white; border: none; border-radius: 5px; cursor: pointer; transition: background 0.3s; }"
                  "button:hover { background-color: #45a049; }"
                  "input[type=number] { width: 60px; padding: 5px; margin: 5px; font-size: 16px; }"
                  "</style>"
                  "<script>"
                  "function updateData() {"
                  "  var xhr = new XMLHttpRequest();"
                  "  xhr.open('GET', '/data', true);"
                  "  xhr.onreadystatechange = function() {"
                  "    if (xhr.readyState == 4 && xhr.status == 200) {"
                  "      var data = JSON.parse(xhr.responseText);"
                  "      document.getElementById('temp').innerHTML = data.temperature + ' °C';"
                  "      document.getElementById('hum').innerHTML = data.humidity + ' %';"
                  "      document.getElementById('soil').innerHTML = data.soil_moisture + ' %';"
                  "      document.getElementById('currentTime').innerHTML = data.current_time;"
                  "    }"
                  "  };"
                  "  xhr.send();"
                  "}"
                  "setInterval(updateData, 2000);"
                  "function sendCommand(endpoint) {"
                  "  var xhr = new XMLHttpRequest();"
                  "  xhr.open('GET', endpoint, true);"
                  "  xhr.send();"
                  "}"
                  "function toggleMode() {"
                  "  var xhr = new XMLHttpRequest();"
                  "  xhr.open('GET', '/toggleMode', true);"
                  "  xhr.onreadystatechange = function() {"
                  "    if (xhr.readyState == 4 && xhr.status == 200) {"
                  "      document.getElementById('mode').innerHTML = xhr.responseText;"
                  "    }"
                  "  };"
                  "  xhr.send();"
                  "}"
                  "function toggleLightMode() {"
                  "  var xhr = new XMLHttpRequest();"
                  "  xhr.open('GET', '/toggleLightMode', true);"
                  "  xhr.onreadystatechange = function() {"
                  "    if (xhr.readyState == 4 && xhr.status == 200) {"
                  "      document.getElementById('lightMode').innerHTML = xhr.responseText;"
                  "    }"
                  "  };"
                  "  xhr.send();"
                  "}"
                  "function updateSchedule() {"
                  "  var hl = document.getElementById('horaLigacao').value;"
                  "  var ml = document.getElementById('minutoLigacao').value;"
                  "  var hd = document.getElementById('horaDesligamento').value;"
                  "  var md = document.getElementById('minutoDesligamento').value;"
                  "  var xhr = new XMLHttpRequest();"
                  "  xhr.open('GET', '/setSchedule?hl=' + hl + '&ml=' + ml + '&hd=' + hd + '&md=' + md, true);"
                  "  xhr.onreadystatechange = function() {"
                  "    if (xhr.readyState == 4 && xhr.status == 200) {"
                  "      alert(xhr.responseText);"
                  "    }"
                  "  };"
                  "  xhr.send();"
                  "}"
                  "</script>"
                  "</head><body>"

                  "<div id='container'>"
                  "<h2>Smart Grow Dashboard</h2>"
                  "<p>Horário atual: <span id='currentTime'>--:--:--</span></p>"
                  "<p>Temperatura: <span id='temp'>--</span></p>"
                  "<p>Umidade do Ar: <span id='hum'>--</span></p>"
                  "<p>Umidade do Solo: <span id='soil'>--</span></p>"
                  "<button onclick=\"sendCommand('/turnLightOn')\">Ligar Iluminação</button>"
                  "<button onclick=\"sendCommand('/turnLightOff')\">Desligar Iluminação</button>"
                  "<button onclick=\"sendCommand('/activatePump')\">Ativar Bomba</button>"
                  "<button onclick=\"sendCommand('/turnCoolersOn')\">Ligar Ventilação</button>"
                  "<button onclick=\"sendCommand('/turnCoolersOff')\">Desligar Ventilação</button>"
                  "<p>Modo da Luz: <span id='lightMode'>Automático</span></p>"
                  "<button onclick='toggleLightMode()'>Alternar Modo Luz</button>"
                  "<p>Modo do Sistema: <span id='mode'>Manual</span></p>"
                  "<button onclick='toggleMode()'>Alternar Modo Sistema</button>"
                  "</div>"

                  "<div id='scheduleContainer'>"
                  "<h2>Agendamento da Luz</h2>"
                  "<p>Hora de Ligar: <input id='horaLigacao' type='number' min='0' max='23' value='18'> : "
                  "<input id='minutoLigacao' type='number' min='0' max='59' value='0'></p>"
                  "<p>Hora de Desligar: <input id='horaDesligamento' type='number' min='0' max='23' value='12'> : "
                  "<input id='minutoDesligamento' type='number' min='0' max='59' value='00'></p>"
                  "<button onclick='updateSchedule()'>Salvar Horários</button>"
                  "</div>"

                  "</body></html>";

    server.send(200, "text/html", html);
}

void handleData() {
    float temp = readTemperature();
    float hum = readHumidity();
    float soil = readSoilMoisture();
    String currentTime = getFormattedTime();

    String json = "{\"current_time\":\"" + currentTime +
                  "\",\"temperature\":" + String(temp) +
                  ",\"humidity\":" + String(hum) +
                  ",\"soil_moisture\":" + String(soil) + "}";

    server.send(200, "application/json", json);
}

// Controles específicos
void handleTurnLightOn() {
    turnLightOn();
    server.send(200, "text/plain", "Luz Ligada");
}

void handleTurnLightOff() {
    turnLightOff();
    server.send(200, "text/plain", "Luz Desligada");
}

void handleTurnCoolersOn() {
    turnCoolersOn();
    server.send(200, "text/plain", "Ventilação Ligada");
}

void handleTurnCoolersOff() {
    turnCoolersOff();
    server.send(200, "text/plain", "Ventilação Desligada");
}

void handleActivatePump() {
    activatePump();
    server.send(200, "text/plain", "Bomba Ativada");
}


// Modos e Agendamento
void handleToggleMode() {
    isAutomaticMode = !isAutomaticMode;
    server.send(200, "text/plain", isAutomaticMode ? "Automático" : "Manual");
}

void handleSetSchedule() {
    if (server.hasArg("hl") && server.hasArg("ml") && server.hasArg("hd") && server.hasArg("md")) {
        horaLigacao = server.arg("hl").toInt();
        minutoLigacao = server.arg("ml").toInt();
        horaDesligamento = server.arg("hd").toInt();
        minutoDesligamento = server.arg("md").toInt();

        server.send(200, "text/plain", "Horários atualizados com sucesso!");
    } else {
        server.send(400, "text/plain", "Erro: Parâmetros inválidos!");
    }

    Serial.println("Horários atualizados:");
    Serial.print("Hora de ligar: ");
    Serial.print(horaLigacao);
    Serial.print(":");
    Serial.println(minutoLigacao);
    Serial.print("Hora de desligar: ");
    Serial.print(horaDesligamento);
    Serial.print(":");
    Serial.println(minutoDesligamento);

}

void handleToggleLightMode() {
    isLightAuto = !isLightAuto;
    server.send(200, "text/plain", isLightAuto ? "Automático" : "Manual");
}
