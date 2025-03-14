#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include "time.h"
#include <ArduinoOTA.h>

#include "secrets.h" // Carrega senha e SSID Wi-Fi

// IP fixo
IPAddress local_IP(192, 168, 15, 200);
IPAddress gateway(192, 168, 15, 1);
IPAddress subnet(255, 255, 255, 0);

// NTP (Configuração de hora)
const char* ntpServer = "216.239.35.12";
const long gmtOffset_sec = -10800;  // Fuso horário de Brasília
const int daylightOffset_sec = 0;


//String hora e data
String getFormattedTime() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Hora não sincronizada! Tentando novamente...");
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
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

//Hora do liga/desliga da luz
int horaLigacao = 18;
int minutoLigacao = 0;
int horaDesligamento = 12;
int minutoDesligamento = 30;



// Sensor DHT22
#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Sensor de umidade do solo
#define SOIL_MOISTURE_PIN 32

// Relés
#define RELAY_PIN_1 5  // Lâmpada
#define RELAY_PIN_3 2  // Coolers
#define RELAY_PIN_4 4  // Bomba

// Servidor Web
WebServer server(80);

// Variáveis globais
bool isAutomaticMode = false;    // Indica se o sistema está no modo automático
unsigned long previousMillisLight = 0;  // Controle de tempo para a luz
unsigned long previousMillisPump = 0;   // Controle de tempo para a bomba de água
unsigned long previousMillisCooler = 0; // Controle de tempo para os coolers
bool isLightOn = false; // Estado atual da luz (ligada ou desligada)

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
}

void setup() {

  delay(2000);

  Serial.begin(115200);

  // Inicializa sensores e relés
  dht.begin();
  pinMode(SOIL_MOISTURE_PIN, INPUT); 
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_3, OUTPUT);
  pinMode(RELAY_PIN_4, OUTPUT);
  digitalWrite(RELAY_PIN_1, HIGH);
  digitalWrite(RELAY_PIN_3, HIGH);
  digitalWrite(RELAY_PIN_4, HIGH);

  // Configuração do IP fixo
  IPAddress gateway(192, 168, 15, 1);
  IPAddress subnet(255, 255, 255, 0);
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("Falha ao configurar IP estático!");
  }

  // Conecta Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Wi-Fi conectado!");
  Serial.print("IP do ESP32: ");
  Serial.println(WiFi.localIP());

  

  // Inicialização OTA
  ArduinoOTA.begin();

  // Inicialização do servidor web
  // Configurações das rotas do servidor
  server.on("/", handleRoot);               // Página principal
  server.on("/data", handleData);           // Dados dos sensores
  server.on("/toggleRelayOn", handleRelayOn);     // Ligar luz
  server.on("/toggleRelayOff", handleRelayOff);   // Desligar luz
  server.on("/activatePump", handleActivatePump); // Ativar bomba
  server.on("/toggleRelay3On", handleRelay3On);   // Ligar ventilação
  server.on("/toggleRelay3Off", handleRelay3Off); // Desligar ventilação
  server.on("/toggleMode", handleToggleMode);     // Alternar modo manual/automático
  server.on("/setSchedule", handleSetSchedule);

  // Sincronização da hora via NTP
Serial.println("Conectando ao servidor NTP...");
if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Conectado ao Wi-Fi, iniciando sincronização NTP...");
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
} else {
    Serial.println("Erro: ESP32 não está conectado ao Wi-Fi!");
}
delay(2000);  // Aguarde um pouco para garantir a sincronização
struct tm timeinfo;
if (!getLocalTime(&timeinfo)) {
    Serial.println("Falha ao obter hora do servidor NTP!");
} else {
    Serial.println("Hora sincronizada com sucesso!");
}

    // Inicializa o servidor Web
  server.begin();
  Serial.println("Servidor iniciado");
}

//Obter hora atual
void getCurrentTime(int &hour, int &minute) {
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

// Função para servir a página principal
void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
                "<style>body { font-family: Arial, sans-serif; background-color: #e09292; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; }"
                "h2 { color: #333; font-size: 24px; }"
                "p { font-size: 18px; color: #555; }"
                "#container { background-color: #fff; padding: 20px; border-radius: 10px; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); text-align: center; }"
                "#temp, #hum, #soil { font-weight: bold; font-size: 22px; color: #4CAF50; }"
                "button { margin-top: 15px; padding: 10px 20px; font-size: 16px; color: white; background-color: #c74b36; border: none; border-radius: 5px; cursor: pointer; }"
                "</style>"
                "<script>"
                "function updateData() {"
                "  var xhr = new XMLHttpRequest();"
                "  xhr.open('GET', '/data', true);"
                "  xhr.onreadystatechange = function() {"
                "    if (xhr.readyState == 4 && xhr.status == 200) {"
                "      var data = JSON.parse(xhr.responseText);"
                "      document.getElementById('temp').innerHTML = data.temperature + ' &deg;C';"
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
                "</script></head><body><div id='container'>"
                "<h2>Smart Grow</h2>"
                "<p>Horário atual: <span id='currentTime'>--:--:--</span></p>"
                "<p>Temperatura: <span id='temp'>--</span></p>"
                "<p>Umidade: <span id='hum'>--</span></p>"
                "<p>Umidade do Solo: <span id='soil'>--</span></p>"
                "<button onclick=\"sendCommand('/toggleRelayOn')\">Ligar Iluminação</button>"
                "<button onclick=\"sendCommand('/toggleRelayOff')\">Desligar Iluminação</button>"
                "<button onclick=\"sendCommand('/activatePump')\">Ativar Bomba</button>"
                "<button onclick=\"sendCommand('/toggleRelay3On')\">Ligar Ventilação</button>"
                "<button onclick=\"sendCommand('/toggleRelay3Off')\">Desligar Ventilação</button>"
                "<p>Modo Atual: <span id='mode'>" + String(isAutomaticMode ? "Automático" : "Manual") + "</span></p>"
                "<button onclick='toggleMode()'>Alternar Modo</button>"
                "</div>"
              "<div id='container'>"
              "<h2>Controle de Horário</h2>"
              "<p>Hora de Ligar: <input id='horaLigacao' type='number' min='0' max='23' value='" + String(horaLigacao) + "'> : "
              "<input id='minutoLigacao' type='number' min='0' max='59' value='" + String(minutoLigacao) + "'></p>"
              "<p>Hora de Desligar: <input id='horaDesligamento' type='number' min='0' max='23' value='" + String(horaDesligamento) + "'> : "
              "<input id='minutoDesligamento' type='number' min='0' max='59' value='" + String(minutoDesligamento) + "'></p>"
              "<button onclick='updateSchedule()'>Salvar Horários</button>"
              "</div></body></html>";

  server.send(200, "text/html", html);
}

// Função para enviar os dados dos sensores
void handleData() {
  float h = dht.readHumidity();         // Lê a umidade
  float t = dht.readTemperature();      // Lê a temperatura
  int soilMoistureValue = analogRead(SOIL_MOISTURE_PIN); // Lê o valor do sensor de umidade do solo
  float soilMoisturePercent = map(soilMoistureValue, 1100, 2700, 100, 0); // Converte para percentual

String currentTime = getFormattedTime();

  // Cria JSON com os dados
String json = "{\"current_time\":\"" + currentTime +
              "\",\"temperature\":" + String(t) + 
              ",\"humidity\":" + String(h) + 
              ",\"soil_moisture\":" + String(soilMoisturePercent) + "}";

            
  server.send(200, "application/json", json);
              }

// Alterna entre os modos manual e automático
void handleToggleMode() {
  isAutomaticMode = !isAutomaticMode; // Alterna o estado do modo
  server.send(200, "text/plain", isAutomaticMode ? "Automático" : "Manual");
}


// Funções para controle dos relés
void handleRelayOn() { digitalWrite(RELAY_PIN_1, LOW); server.send(200, "text/plain", "Luz Ligada"); }
void handleRelayOff() { digitalWrite(RELAY_PIN_1, HIGH); server.send(200, "text/plain", "Luz Desligada"); }
void handleRelay3On() { digitalWrite(RELAY_PIN_3, LOW); server.send(200, "text/plain", "Ventilação Ligada"); }
void handleRelay3Off() { digitalWrite(RELAY_PIN_3, HIGH); server.send(200, "text/plain", "Ventilação Desligada"); }
void handleActivatePump() { digitalWrite(RELAY_PIN_4, LOW); delay(1500); digitalWrite(RELAY_PIN_4, HIGH); server.send(200, "text/plain", "Bomba Ativada"); }

// Loop principal
void loop() {
  ArduinoOTA.handle();  // Gerencia as atualizações OTA
  server.handleClient(); // Gerencia os pedidos do cliente


  int currentHour, currentMinute;
    getCurrentTime(currentHour, currentMinute);
    if (currentHour == -1) return; // Evita erro se a hora não foi obtida

    // Liga luz às 18:00 e desliga às 12:30 (exemplo com minutos)
    if ((currentHour > horaLigacao) || (currentHour == horaLigacao && currentMinute >= minutoLigacao) ||  
    (currentHour < horaDesligamento) || (currentHour == horaDesligamento && currentMinute < minutoDesligamento)) {
    digitalWrite(RELAY_PIN_1, LOW);  // Liga a lâmpada
} else {
    digitalWrite(RELAY_PIN_1, HIGH); // Desliga a lâmpada
}




  // Funções do modo automático
  if (isAutomaticMode) {
    unsigned long currentMillis = millis();

    // Controle da luz (40s ligada, 20s desligada)
    if (currentMillis - previousMillisLight >= (isLightOn ? 40000 : 20000)) {
      isLightOn = !isLightOn;
      digitalWrite(RELAY_PIN_1, isLightOn ? LOW : HIGH);
      previousMillisLight = currentMillis;
    }

    // Controle da bomba de água (umidade abaixo de 40% por 5s)
    int soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);
    float soilMoisturePercent = map(soilMoistureValue, 1100, 2700, 100, 0);
    if (soilMoisturePercent < 40 && currentMillis - previousMillisPump >= 5000) {
      digitalWrite(RELAY_PIN_4, LOW);
      delay(1500);
      digitalWrite(RELAY_PIN_4, HIGH);
      previousMillisPump = currentMillis;
    }

    // Controle dos coolers (temperatura > 28°C ou < 25°C)
    float temperature = dht.readTemperature();
    if (temperature > 28) {
      digitalWrite(RELAY_PIN_3, LOW);
      previousMillisCooler = currentMillis;
    } else if (temperature < 25 && currentMillis - previousMillisCooler >= 5000) {
      digitalWrite(RELAY_PIN_3, HIGH);
    }
  }
}