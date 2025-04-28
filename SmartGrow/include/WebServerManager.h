#ifndef WEB_SERVER_MANAGER_H
#define WEB_SERVER_MANAGER_H

void initWiFi();
void initWebServer();
void handleWebServer();
void handleOTA();

// Expondo as variáveis globais
extern bool isAutomaticMode;
extern bool isLightAuto;
extern int horaLigacao;
extern int minutoLigacao;
extern int horaDesligamento;
extern int minutoDesligamento;

#endif // WEB_SERVER_MANAGER_H
