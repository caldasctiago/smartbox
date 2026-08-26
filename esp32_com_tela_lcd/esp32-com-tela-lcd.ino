#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Configuração do Display LCD (Endereço comum: 0x27 ou 0x3F)
LiquidCrystal_I2C lcd(0x27, 20, 4);

// --- MAC ADDRESS EXATO DO SEU ESP32 TRANSMISSOR ---
uint8_t enderecoTransmissor[] = {0xEC, 0xE3, 0x34, 0x48, 0x34, 0x50};

// MESMA chave secreta definida no transmissor (16 caracteres exatos)
uint8_t chaveSecreta[16] = {'S', 'i', 'l', 'o', 'S', 'e', 'g', 'u', 'r', 'o', '1', '2', '3', '4', '5', '6'};

// Estrutura de dados recebidos
typedef struct struct_mensagem {
  int distancia_cm;
} struct_mensagem;

struct_mensagem dadosRecebidos;
esp_now_peer_info_t peerInfo;

// Função de callback para receber e descriptografar os dados
void AoReceber(const esp_now_recv_info *info, const uint8_t *dados_recebidos, int len) {
  memcpy(&dadosRecebidos, dados_recebidos, sizeof(dadosRecebidos));
  
  Serial.print("Distância recebida: ");
  Serial.print(dadosRecebidos.distancia_cm);
  Serial.println(" cm");

  // Atualiza o Display LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Nivel do Silo");
  lcd.setCursor(0, 1);
  lcd.print("Dist: ");
  lcd.print(dadosRecebidos.distancia_cm);
  lcd.print(" cm   ");
}

void setup() {
  Serial.begin(115200);
  
  // Inicializa o LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Base Pronta!");
  lcd.setCursor(0, 1);
  lcd.print("Aguardando sinal...");
  
  // Configura o Wi-Fi em modo Estação
  WiFi.mode(WIFI_STA);

  // Inicializa o ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Erro ao inicializar ESP-NOW");
    return;
  }
  
  // Registra a função de callback de recepção
  esp_now_register_recv_cb(AoReceber);

  // --- CONFIGURAÇÃO DO PEER CRIPTOGRAFADO NA BASE ---
  memcpy(peerInfo.peer_addr, enderecoTransmissor, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = true;
  memcpy(peerInfo.lmk, chaveSecreta, 16);

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Falha ao registrar peer criptografado na Base");
  } else {
    Serial.println("Transmissor seguro registrado com sucesso na Base!");
  }
}

void loop() {
  // A base fica permanentemente escutando
}