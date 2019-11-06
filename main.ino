#include <SSD1306.h>

// Instância do display
SSD1306 display(0x3c, 4, 15);

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

void setup() {
  Serial.begin(9600);

  initializeDisplay();
}

void loop() {
  display.drawString(0, 0, "Monitorando...");
  display.display();
}
