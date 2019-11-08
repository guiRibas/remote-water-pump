#include <SSD1306.h>

// Instância do display
SSD1306 display(0x3c, 4, 15);

// Pinos de limite
#define PIN_MIN_LIMIT 23
#define PIN_MAX_LIMIT 17

// Pinos de status
#define PIN_LED_OK 13
#define PIN_LED_ERR 12

void initializeDisplay(){
  //O estado do GPIO16 é utilizado para controlar o display
  pinMode(16, OUTPUT);
  //Reseta as configurações do display
  digitalWrite(16, LOW);
  //Para o display ficar ligado
  digitalWrite(16, HIGH);

  //Configurações do display
  display.init();
  display.flipScreenVertically();
  display.setContrast(255);
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
}

void welcomeMessage(){
  display.clear();
  
  display.drawString(0, 0, "Sistema de bombeamento");
  display.drawString(0, 15, "d'água a longa");
  display.drawString(0, 30, "distância");
  display.drawString(0, 50, "Faculdade Guarapuava");

  display.display();

  delay(6000);

  display.clear();
}

void setup() {
  Serial.begin(9600);

  pinMode(PIN_MIN_LIMIT, INPUT);
  pinMode(PIN_MAX_LIMIT, INPUT);

  pinMode(PIN_LED_OK, OUTPUT);
  pinMode(PIN_LED_ERR, OUTPUT);

  initializeDisplay();
  welcomeMessage();
}

void loop() {
  display.drawString(0, 0, "Monitorando...");
  display.display();
}
