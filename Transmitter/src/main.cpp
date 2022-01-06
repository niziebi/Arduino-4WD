// === Arduino UNO nRF24L01 Transmitter ===
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(8, 10); // (CE 8, CSN 10) MOSI 11, MISO 12, SCK 13
const byte address[6] = "00001";
const int CHANNEL = 74;
int potX;
int potY;
int buttonK;
struct Data_Package {
  byte joystickX;
  byte joystickY;
  byte buttonK;
};
Data_Package data;

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(CHANNEL); // set channel (0-125)
  radio.stopListening();
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(2, INPUT_PULLUP);
}
void loop() {
  potX = map (analogRead(A1), 0, 1023, 0, 255);
  potY = map (analogRead(A0), 0, 1023, 0, 255);
  buttonK = !digitalRead(2);
  // Data Package  
  data.joystickX = potX;
  data.joystickY = potY;
  data.buttonK = buttonK;
  // send data
  radio.write(&data, sizeof(Data_Package));
  // serial monitor
  Serial.print(F("\r\njoystickX: "));
  Serial.print(data.joystickX);
  Serial.print(F("\tjoystickY: "));
  Serial.print(data.joystickY);
  Serial.print(F("\tbuttonK: "));
  Serial.print(data.buttonK);
}
