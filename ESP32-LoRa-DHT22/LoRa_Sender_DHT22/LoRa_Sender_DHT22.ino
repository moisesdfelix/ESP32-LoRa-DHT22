#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include "SSD1306.h"
#include "board_def.h"

#define OLED_ADDRESS 0x3c
#define I2C_SDA 21
#define I2C_SCL 22


SSD1306Wire display(OLED_ADDRESS, I2C_SDA, I2C_SCL, GEOMETRY_128_64);

#include <DHT.h>

#define DHTPIN 13     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

DHT dht(DHTPIN, DHTTYPE);

unsigned long intervalCall1 = 3000;
unsigned long intervalCall2 = 10000;
unsigned long lastCall[2] = {-intervalCall1,-intervalCall2};
float t,h;

void setup(){
  Serial.begin(115200);
  dht.begin();
  while (!Serial);
  if (OLED_RST > 0) {
    pinMode(OLED_RST, OUTPUT);
    digitalWrite(OLED_RST, HIGH);
    delay(100);
    digitalWrite(OLED_RST, LOW);
    delay(100);
    digitalWrite(OLED_RST, HIGH);
  }

  display.init();
  display.flipScreenVertically();
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(display.getWidth() / 2, display.getHeight() / 2, LORA_SENDER ? "LoRa Sender" : "LoRa Receiver");
  display.display();
  delay(2000);


  String info = "Inicializando";
  if (info != "") {
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(display.getWidth() / 2, display.getHeight() / 2, info);
    display.display();
    delay(2000);
  }

  SPI.begin(CONFIG_CLK, CONFIG_MISO, CONFIG_MOSI, CONFIG_NSS);
  LoRa.setPins(CONFIG_NSS, CONFIG_RST, CONFIG_DIO0);
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  if (!LORA_SENDER) {
    display.clear();
    display.drawString(display.getWidth() / 2, display.getHeight() / 2, "LoraRecv Ready");
    display.display();
  }
}


void loop(){
  if(millis() > lastCall[0] + intervalCall1){
    h = dht.readHumidity();
    t = dht.readTemperature();

    if(isnan(h) || isnan(t)){
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
    lastCall[0] = millis();
  }

  
#if LORA_SENDER
  


  //Rotina de liga-desliga
  if(millis() > lastCall[1] + intervalCall2){

    String dados = "T=";
    dados += t;
    dados += " / H=";
    dados += h;
    Serial.print(dados);
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(display.getWidth() / 2, display.getHeight() / 2, dados);
    display.display();
    LoRa.beginPacket();
    LoRa.print(dados);
    LoRa.endPacket();
    lastCall[1] = millis();
  }
#endif
}
