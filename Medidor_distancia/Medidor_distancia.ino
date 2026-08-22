#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Configuração do Display LCD (Endereço I2C 0x27 ou 0x3F, 20 colunas, 4 linhas)
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Definição dos pinos do HC-SR04
const int pinoTrig = 5;
const int pinoEcho = 18;

void setup() {
  Serial.begin(115200);

  // Configuração dos pinos do sensor
  pinMode(pinoTrig, OUTPUT);
  pinMode(pinoEcho, INPUT);

  // Inicializa o LCD
  lcd.init();                      
  lcd.backlight();
  
  lcd.setCursor(0, 0);
  lcd.print("PROTOTIPO: ESP32");
  lcd.setCursor(0, 1);
  lcd.print("Sensor: HC-SR04");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Envia um pulso ultrassônico de 10 microssegundos
  digitalWrite(pinoTrig, LOW);
  delayMicroseconds(2);
  digitalWrite(pinoTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinoTrig, LOW);

  // Lê o tempo que o som levou para ir e voltar (em microssegundos)
  long duracao = pulseIn(pinoEcho, HIGH);

  // Calcula a distância em centímetros
  // Velocidade do som é ~0.0343 cm/us. Dividimos por 2 porque o som vai e volta.
  float distanciaCm = duracao * 0.0343 / 2.0;

  // Exibe as informações fixas no Display 20x4
  lcd.setCursor(0, 0);
  lcd.print("PROTOTIPO: ESP32");
  
  lcd.setCursor(0, 1);
  lcd.print("Sensor: HC-SR04");

  lcd.setCursor(0, 2);
  lcd.print("Distancia: ");

  lcd.setCursor(0, 3);
  // Limite prático do HC-SR04 costuma ser de ~2 a 400 cm
  if (distanciaCm > 2 && distanciaCm < 400) {
    lcd.print(distanciaCm);
    lcd.print(" cm       "); // Espaços extras para limpar caracteres antigos
  } else {
    lcd.print("Fora de alcance ");
  }

  delay(500); // Atualiza a leitura a cada meio segundo
}