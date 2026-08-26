#include <esp_now.h>
#include <WiFi.h>

// --- PINOS DOS 3 SENSORES HC-SR04 ---
const int pinoTrig1 = 5;  
const int pinoEcho1 = 18; 

const int pinoTrig2 = 14;  
const int pinoEcho2 = 12; 

const int pinoTrig3 = 4;  
const int pinoEcho3 = 2; 

// --- MAC ADDRESS DO RECEPTOR (BASE) ---
uint8_t enderecoBase[] = {0x8C, 0x94, 0xDF, 0xB9, 0x63, 0x04};

// Chave secreta de 16 bytes para criptografia AES
uint8_t chaveSecreta[16] = {'S', 'i', 'l', 'o', 'S', 'e', 'g', 'u', 'r', 'o', '1', '2', '3', '4', '5', '6'};

// Estrutura de dados atualizada para 3 sensores
typedef struct struct_mensagem {
  int distancia1_cm;
  int distancia2_cm;
  int distancia3_cm;
} struct_mensagem;

struct_mensagem dadosSensor;
esp_now_peer_info_t peerInfo;

// Tempo em Deep Sleep (10 segundos para testes)
#define TEMPO_DORMIR 10000000 

int medirDistancia(int pinoTrig, int pinoEcho) {
  digitalWrite(pinoTrig, LOW);
  delayMicroseconds(2);
  digitalWrite(pinoTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinoTrig, LOW);
  
  long duracao = pulseIn(pinoEcho, HIGH, 30000); // Timeout de 30ms
  if (duracao == 0) return 999; // Retorna 999 se não houver eco
  
  int distancia = duracao * 0.034 / 2;
  return distancia;
}

// Callback de status de envio
void AoEnviar(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  Serial.print("Status de Envio: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Sucesso!" : "Falha na entrega.");
}

void setup() {
  Serial.begin(115200);
  
  pinMode(pinoTrig1, OUTPUT);
  pinMode(pinoEcho1, INPUT);
  pinMode(pinoTrig2, OUTPUT);
  pinMode(pinoEcho2, INPUT);
  pinMode(pinoTrig3, OUTPUT);
  pinMode(pinoEcho3, INPUT);
  
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Erro ao iniciar ESP-NOW");
    return;
  }

  esp_now_register_send_cb(AoEnviar);

  // Configura os dados do peer (Base) com criptografia
  memcpy(peerInfo.peer_addr, enderecoBase, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = true;
  memcpy(peerInfo.lmk, chaveSecreta, 16);
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Falha ao adicionar a Base");
    return;
  }

  // Faz a leitura dos 3 sensores
  dadosSensor.distancia1_cm = medirDistancia(pinoTrig1, pinoEcho1);
  dadosSensor.distancia2_cm = medirDistancia(pinoTrig2, pinoEcho2);
  dadosSensor.distancia3_cm = medirDistancia(pinoTrig3, pinoEcho3);
  
  Serial.print("Silo 1: "); Serial.print(dadosSensor.distancia1_cm); Serial.println(" cm");
  Serial.print("Silo 2: "); Serial.print(dadosSensor.distancia2_cm); Serial.println(" cm");
  Serial.print("Silo 3: "); Serial.print(dadosSensor.distancia3_cm); Serial.println(" cm");
  
  // Envia via ESP-NOW
  esp_now_send(enderecoBase, (uint8_t *) &dadosSensor, sizeof(dadosSensor));
  
  delay(100); // Tempo para o rádio transmitir

  // Entra em Deep Sleep
  Serial.println("Entrando em Deep Sleep por 10 segundos...\n");
  esp_sleep_enable_timer_wakeup(TEMPO_DORMIR);
  esp_deep_sleep_start();
}

void loop() {
  // Nada aqui
}