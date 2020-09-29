#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include "SSD1306.h"
#include "board_def.h"

#define OLED_ADDRESS 0x3c
#define I2C_SDA 21
#define I2C_SCL 22

SSD1306Wire display(OLED_ADDRESS, I2C_SDA, I2C_SCL, GEOMETRY_128_64);

void setup(){
  Serial.begin(115200);
  while (!Serial);
  if (OLED_RST > 0){
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

int count = 0;

void loop(){
#if LORA_SENDER
#else
  String dados;
  if (LoRa.parsePacket()){
    String recv = "";
    while (LoRa.available()) {
      recv += (char)LoRa.read();
    }
    count++;
    dados = count;
    dados += ". ";
    dados += recv;
    display.clear();
    display.drawString(display.getWidth() / 2, display.getHeight() / 2, dados);
    display.display();
    }
#endif
}


  
