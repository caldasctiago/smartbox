#include <WiFi.h>

void setup(){
  Serial.begin(115200);
  
  // Dá 2 segundos para a placa e o computador se estabilizarem
  delay(2000); 
  
  Serial.println("\nAcordando a antena Wi-Fi...");
  
  // Força o modo estação e limpa qualquer configuração travada
  WiFi.mode(WIFI_MODE_STA);
  WiFi.disconnect();
  
  // Espera mais meio segundo para o rádio ligar de fato
  delay(500);

  Serial.println("--- ENDERECO MAC DESTE ESP32 ---");
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
}

void loop(){
  // Nada aqui
}