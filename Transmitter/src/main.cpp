// === Arduino UNO nRF24L01 Transmitter ===
// (CE 8, CSN 10) MOSI 11, MISO 12, SCK 13
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const int BUTTON_PIN = 2;

const int CHANNEL = 74;
const byte address[6] = "00001";

int x;
int y;
int button;

struct Data {
  byte x;
  byte y;
  byte button;
};

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
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  x = map(analogRead(A1), 0, 1023, 0, 255);
  y = map(analogRead(A0), 0, 1023, 0, 255);
  button = !digitalRead(BUTTON_PIN);
  data.x = x;
  data.y = y;
  data.button = button;
  radio.write(&data, sizeof(Data));
}