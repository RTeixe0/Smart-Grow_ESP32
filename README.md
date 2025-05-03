# Smart-Grow_ESP32: Sua Estufa Inteligente e Automatizada

![ESP32](https://img.shields.io/badge/ESP32-000000?style=for-the-badge&logo=espressif&logoColor=white)
![Arduino](https://img.shields.io/badge/Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white)
![WiFi](https://img.shields.io/badge/WiFi-1ABCFE?style=for-the-badge&logo=wifi&logoColor=white)


Cansado de se preocupar se suas plantas estão recebendo água, luz e ventilação adequadas? O Smart-Grow_ESP32 ajuda você a monitorar e automatizar seu cultivo indoor!

## O Que Faz? 🤔

O Smart-Grow funciona como um centro de controle para sua estufa. Ele usa sensores para monitorar:

*   🌡️ **Temperatura e Umidade do Ar:** Fique de olho no clima da sua estufa.
*   💧 **Umidade do Solo:** Saiba quando suas plantas precisam de água.

Com base nesses dados e nas suas configurações, ele controla automaticamente:

*   💡 **Luzes:** Liga e desliga nos horários que você definir.
*   💨 **Coolers/Ventilação:** Ativa os coolers se a temperatura subir muito (> 28°C) ou intercala períodos de ventilação para circulação de ar, dependendo se a luz está acesa ou apagada.

Através de uma interface web simples 💻, você também pode:

*   📊 **Ver os Dados:** Acompanhe as leituras dos sensores em tempo real.
*   🕹️ **Controlar Manualmente:**
    *   Ligar/Desligar as **Luzes** (se o modo automático estiver desativado).
    *   Ligar/Desligar os **Coolers/Ventilação** (se o modo automático estiver desativado).
    *   Ativar a **Bomba d'água** 🚿 por um curto período (controle apenas manual).
*   ⏰ **Agendar a Iluminação:** Defina os horários para ligar e desligar as luzes automaticamente.
*   ⚙️ **Alternar Modos:** Escolha entre controle **Automático** ou **Manual** tanto para as **Luzes** quanto para os **Coolers**.

**Importante:** O controle automático da **bomba d'água** 🚿 baseado na umidade do solo ainda não está implementado; seu acionamento é apenas manual pela interface web.

## Por Que Usar? 👍

*   🤖 **Automação Inteligente:** Deixe o sistema cuidar da luz e da ventilação automaticamente.
*   👀 **Monitoramento Fácil:** Tenha acesso rápido às condições da sua estufa.
*   🎛️ **Controle Flexível:** Alterne entre automático e manual conforme sua necessidade.
*   🌐 **Acionamento Remoto:** Controle luzes, ventilação e bomba de onde estiver (na sua rede local).
*   🧑‍🌾 **Para Todos:** Útil para iniciantes e experts, facilitando o cultivo indoor.

## Como Funciona por Dentro (Tecnologias) 🛠️

Usamos o poderoso **ESP32** como coração do sistema, programado em **C++ (Arduino)**. Sensores como **DHT22** (temperatura/umidade do ar) e **sensor capacitivo** (umidade do solo) coletam os dados. **Relés** controlam os equipamentos (luz, coolers, bomba). Tudo se conecta via **Wi-Fi**, e um **servidor web** simples permite o monitoramento e controle.

## Como Começar 🚀

1.  **Monte o Hardware:** ESP32, sensores, relés, etc.
2.  **Prepare o Ambiente:** Instale a Arduino IDE com suporte ao ESP32.
3.  **Baixe o Código:** Clone ou faça o download deste repositório.
4.  **Configure:** Edite `secrets.h` (crie a partir do `secrets_example.h`) com seu Wi-Fi. Verifique os pinos em `config.h`.
5.  **Carregue o Código:** Compile e envie para o ESP32.
6.  **Acesse:** Descubra o IP do ESP32 na sua rede e acesse pelo navegador.

## Próximos Passos 🔮

O projeto tem potencial! Planos incluem implementar controle automático para a bomba baseado nos sensores, integração com nuvem, app mobile, suporte a mais sensores e otimizações.

## Contato 📫

Dúvidas ou sugestões? Abra uma issue aqui no GitHub ou fale com o dev:

*   **LinkedIn:** [Renan Teixeira](https://www.linkedin.com/in/renaneteixeira/)
*   **Email:** renanteixeira121@outlook.com

