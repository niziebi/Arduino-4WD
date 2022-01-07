// Arduino-4WD 送信機(コントローラー)
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// 各種ピンの定数
const int BUTTON_PIN = 2;

// 通信に必要な定数
const int CHANNEL = 74;
const byte address[6] = "00001";

// 通信データ
struct Data {
  int joystickX;
  int joystickY;
  boolean button;
};

// 通信用オブジェクト
Data data;
RF24 radio(8, 10);

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(CHANNEL);
  radio.stopListening();
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  data.joystickX = analogRead(A1);
  data.joystickY = analogRead(A0);
  data.button = !digitalRead(BUTTON_PIN);
  radio.write(&data, sizeof(Data));
}