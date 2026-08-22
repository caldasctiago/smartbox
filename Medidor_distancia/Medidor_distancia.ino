#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

// HC-SR04
const int pinoTrig = 5;
const int pinoEcho = 18;

// Botão para definir ZERO
const int pinoBotao = 15;

// Variável para guardar o valor de referência (box vazio)
float distanciaZero = -1;  // -1 significa "não calibrado"

// Raio da base circular (em metros)
float raioBase = 0.105; // valor do raio da base (m)

void setup() {
  Serial.begin(115200);

  pinMode(pinoTrig, OUTPUT);
  pinMode(pinoEcho, INPUT);

  pinMode(pinoBotao, INPUT);  // usando pull-down externo

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("ESP32 - Nivel Box");
  lcd.setCursor(0, 1);
  lcd.print("Press ZERO Btn");
}

void loop() {

  // --- LEITURA DO SENSOR ---
  digitalWrite(pinoTrig, LOW);
  delayMicroseconds(2);
  digitalWrite(pinoTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinoTrig, LOW);

  long duracao = pulseIn(pinoEcho, HIGH);
  float distanciaAtual = duracao * 0.0343 / 2.0;

  // --- BOTAO PRESSIONADO PARA DEFINIR ZERO ---
  if (digitalRead(pinoBotao) == HIGH) {
    distanciaZero = distanciaAtual;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ZERO definido!");
    lcd.setCursor(0, 1);
    lcd.print("ZERO = ");
    lcd.print(distanciaZero);
    delay(1500);
  }

  // --- EXIBIÇÃO NO LCD ---
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dist Atual: ");
  lcd.print(distanciaAtual);

  if (distanciaZero < 0) {
    lcd.setCursor(0, 2);
    lcd.print("ZERO nao definido");
    Serial.println("Pressione o botao ZERO");
  } else {
    float alturaMaterial = distanciaZero - distanciaAtual;

    // converter altura para metros
    float alturaMaterial_m = alturaMaterial / 100.0;

    // calcular volume do cilindro
    float volume_m3 = 3.14159 * raioBase * raioBase * alturaMaterial_m;

    lcd.setCursor(0, 1);
    lcd.print("Alt: ");
    lcd.print(alturaMaterial);
    lcd.print("cm");

    lcd.setCursor(0, 2);
    lcd.print("Vol: ");
    lcd.print(volume_m3, 3);
    lcd.print(" m3");

    Serial.print("Altura: ");
    Serial.print(alturaMaterial);
    Serial.print(" cm | Volume: ");
    Serial.print(volume_m3);
    Serial.println(" m3");
  }

  delay(500);
}
