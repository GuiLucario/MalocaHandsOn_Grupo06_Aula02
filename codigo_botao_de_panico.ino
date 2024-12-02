#include <WiFi.h> // Biblioteca para conectar o ESP32 ao WiFi
#include <HTTPClient.h> // Biblioteca para fazer requisições HTTP
#include <UrlEncode.h> // Biblioteca para codificar texto em formato de URL

// Definição das credenciais do WiFi
const char* ssid = "Starlink_CIT"; // Nome da rede WiFi
const char* password = "Ufrr@2024Cit"; // Senha do WiFi

// Definição dos pinos do botão e LEDs
#define botao 21 // Pino do botão
#define led1 23  // LED externo (acende ao enviar mensagem)
#define led2 2   // LED interno do ESP32 (indica status de conexão)

bool flag = 1; // Variável para controlar o envio da mensagem
int clickCount = 0; // Contador de cliques
unsigned long lastClickTime = 0; // Tempo do último clique

// Número de telefone e chave da API para CallMeBot
String phoneNumber1 = "559581151845"; // Número de telefone (Brasil 55)
String apiKey1 = "1042069"; // Chave de API do CallMeBot
String phoneNumber2 = "X";  // Outro número de telefone
String apiKey2 = "Y";  // Outra chave de API

// Função para enviar a mensagem via CallMeBot API
void sendMessage(String phoneNumber, String apiKey, String message) {
    String url = "https://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKey + "&text=" + urlEncode(message);
    HTTPClient http; // Objeto HTTPClient
    http.begin(url); // Inicia a conexão com a URL
    http.addHeader("Content-Type", "application/x-www-form-urlencoded"); // Cabeçalho HTTP
    int httpResponseCode = http.POST(url); // Envia a requisição POST
    if (httpResponseCode == 200) {
        Serial.println("Mensagem enviada com sucesso");
    } else {
        Serial.println("Erro no envio da mensagem");
        Serial.print("HTTP response code: ");
        Serial.println(httpResponseCode);
    }
    http.end(); // Libera os recursos da requisição HTTP
}

// Função de debounce simples para o botão
bool debounceBotao(int pin) {
    int leitura = digitalRead(pin);
    if (leitura == LOW) {
        delay(50); // Atraso para debouncing
        if (digitalRead(pin) == LOW) {
            return true; // Retorna true se o botão for pressionado
        }
    }
    return false; // Retorna false se o botão não foi pressionado
}

void setup() {
    Serial.begin(115200); // Comunicação serial
    pinMode(botao, INPUT_PULLUP); // Pino do botão com pull-up interno
    pinMode(led1, OUTPUT); // LED1 como saída
    pinMode(led2, OUTPUT); // LED2 como saída
    digitalWrite(led1, LOW); // Inicialmente LEDs apagados
    digitalWrite(led2, LOW);
    WiFi.begin(ssid, password); // Conecta ao Wi-Fi

    Serial.println("Conectando");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print("."); // Indicando que está tentando se conectar
        digitalWrite(led2, !digitalRead(led2)); // Pisca o LED2 enquanto conecta
    }
    Serial.println("");
    Serial.print("Conectado ao WiFi neste IP: ");
    digitalWrite(led2, HIGH); // LED2 aceso indica conexão bem-sucedida
    Serial.println(WiFi.localIP()); // Exibe o IP local do ESP32
}

void loop() {
    if (debounceBotao(botao)) { // Verifica se o botão foi pressionado
        clickCount++; // Incrementa o contador de cliques
        lastClickTime = millis(); // Atualiza o tempo do último clique
        Serial.print("Cliques: ");
        Serial.println(clickCount); // Exibe o número de cliques
        delay(500); // LED1 ficará aceso por 5 segundos
        
        if (clickCount == 2) { // Se houver 2 cliques
            Serial.println("Emergência acionada! Enviando mensagem...");
            sendMessage(phoneNumber1, apiKey1, "Emergência acionada!");
            sendMessage(phoneNumber2, apiKey2, "SOS: Preciso de ajuda!");

            digitalWrite(led1, HIGH); // Acende o LED1 como indicação de emergência
            delay(5000); // LED1 ficará aceso por 5 segundos
            digitalWrite(led1, LOW); // Apaga o LED1 após 5 segundos

            clickCount = 0; // Zera o contador de cliques
        }
    }

    // Se o botão não for pressionado por 5 segundos, reseta o contador de cliques
    if (millis() - lastClickTime > 5000 && clickCount > 0) {
        clickCount = 0; // Reseta o contador após 5 segundos
        Serial.println("Contador de cliques resetado.");
    }
}
