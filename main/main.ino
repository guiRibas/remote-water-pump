#include <SSD1306.h>
#include <EEPROM.h>
#include <SPI.h>
#include <LoRa.h>

// Pinos do lora
#define SCK 5   // GPIO5
#define MISO 19 // GPIO19
#define MOSI 27 // GPIO27
#define SS 18   // GPIO18
#define RST 14  // GPIO14
#define DI00 26 // GPIO26 (Interrupt Request)

// Frequência de comunicação, 868MHz
#define BAND 868E6

// Instância do display
SSD1306 display(0x3c, 4, 15);

// Número de bytes que seram acessados
#define EEPROM_SIZE 2

// Pinos de limite
#define PIN_MIN_LIMIT 23
#define PIN_MAX_LIMIT 17

// Pinos de status
#define PIN_LED_OK 13
#define PIN_LED_ERR 12

void initializeDisplay()
{
  //O estado do GPIO16 é utilizado para controlar o display
  pinMode(16, OUTPUT);
  //Reseta as configurações do display
  digitalWrite(16, LOW);
  //Para o display ficar ligado
  digitalWrite(16, HIGH);

  //Configurações do display
  bool displayStatus = display.init();
  display.flipScreenVertically();
  display.setContrast(255);
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  if (!displayStatus)
  {
    display.clear();
    display.setFont(ArialMT_Plain_24);
    display.drawString(0, 0, "ALERTA!");
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 30, "Erro ao inicializar o Display!");
    display.display();

    digitalWrite(PIN_LED_ERR, HIGH);

    while(1);
  }
}

void welcomeMessage()
{
  display.clear();
  display.drawString(0, 0, "Sistema de bombeamento");
  display.drawString(0, 15, "d'água a longa");
  display.drawString(0, 30, "distância");
  display.drawString(0, 50, "Faculdade Guarapuava");
  display.display();

  delay(6000);

  display.clear();
}

void monitoringMinLimit()
{
  if (digitalRead(PIN_MIN_LIMIT) == LOW && EEPROM.read(0) != 0)
  {
    EEPROM.write(0, 0);
    EEPROM.commit();

    Serial.println("Ligar Motor");
    sendPacket(1);
  }
  
  if (digitalRead(PIN_MIN_LIMIT) == HIGH && EEPROM.read(0) == 0)
  {
    EEPROM.write(0, 1);
    EEPROM.commit();
  }
}

void monitoringMaxLimit()
{
  if (digitalRead(PIN_MAX_LIMIT) == HIGH && EEPROM.read(1) != 0)
  {
    EEPROM.write(1, 0);
    EEPROM.commit();
    
    Serial.println("Desligar Motor");
    sendPacket(0);
  }
  
  if (digitalRead(PIN_MIN_LIMIT) == LOW && EEPROM.read(1) == 0)
  {
    EEPROM.write(1, 1);
    EEPROM.commit();
  }
}

void setupLoRa()
{ 
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DI00);

  if (!LoRa.begin(BAND))
  {
    display.clear();
    display.setFont(ArialMT_Plain_24);
    display.drawString(0, 0, "ALERTA!");
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 30, "Erro ao inicializar o LoRa!");
    display.display();

    digitalWrite(PIN_LED_ERR, HIGH);
    
    while(1);
  }

  LoRa.enableCrc();

  digitalWrite(PIN_LED_ERR, LOW);
  digitalWrite(PIN_LED_OK, HIGH);
}

void sendPacket(int floaterStatus)
{
  if (!LoRa.beginPacket())
  {
    display.clear();
    display.drawString(0, 30, "LoRa não está pronto!");
    display.display();

    digitalWrite(PIN_LED_OK, LOW);
    digitalWrite(PIN_LED_ERR, HIGH);
    
    while(1);
  }
  
  LoRa.print("="); //delimitador
  LoRa.print(floaterStatus);
  LoRa.print("#"); //delimitador
  
  if (!LoRa.endPacket())
  {
    display.clear();
    display.drawString(0, 30, "Erro ao enviar dados!");
    display.display();

    digitalWrite(PIN_LED_OK, LOW);
    digitalWrite(PIN_LED_ERR, HIGH);

    while(1);
  }
}

void setup()
{
  Serial.begin(9600);

  pinMode(PIN_MIN_LIMIT, INPUT);
  pinMode(PIN_MAX_LIMIT, INPUT);

  pinMode(PIN_LED_OK, OUTPUT);
  pinMode(PIN_LED_ERR, OUTPUT);

  EEPROM.begin(EEPROM_SIZE);
  EEPROM.commit();

  initializeDisplay();
  welcomeMessage();
  setupLoRa();
}

void loop()
{
  display.drawString(0, 0, "Monitorando...");
  display.display();

  monitoringMinLimit();
  delay(2);
  monitoringMaxLimit();

  delay(1);
}
