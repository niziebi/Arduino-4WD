// === Arduino UNO nRF24L01 Receiver ===
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(8, 10); // (CE 8, CSN 10) MOSI 11, MISO 12, SCK 13
const byte address[6] = "00001";
const int enA = 3;
const int in1 = 2;
const int in2 = 4;
const int in3 = 7;
const int in4 = 8;
const int enB = 5;
const int LEDR = A3;
const int LEDG = A4;
const int CHANNEL = 74;
unsigned long previousTime = 0;
int xAxis = 510;
int yAxis = 510;
int buttonK;
int speedLeft = 0;
int speedRight = 0;
struct Data_Package {
  byte joystickX;
  byte joystickY;
  byte buttonK;
};
Data_Package data;

void setup() {
  Serial.begin(115200);
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(CHANNEL); // set channel (0-125)
  radio.startListening();
}
void loop() {
  unsigned long currentTime = millis();
  // Received Data Package
  if (radio.available()) {
    previousTime = currentTime;
    radio.read(&data, sizeof(Data_Package));
    xAxis = map (data.joystickX, 0, 255, 0, 1023);
    yAxis = map (data.joystickY, 0, 255, 0, 1023);
    buttonK = data.buttonK;
  }
  else {
    // Checking Radio available
    const long presetTime = 80; // milli second
    if (currentTime >= (previousTime + presetTime)) {
      xAxis = 510;
      yAxis = 510;
      buttonK = 0;
    }
  }
  // Forward
  if (yAxis > 550) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    speedLeft  = map(yAxis, 510, 1023, 0, 255);
    speedRight = map(yAxis, 510, 1023, 0, 255);
  }
  // Backward
  if (yAxis < 470) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    speedLeft  = map(yAxis, 510, 0, 0, 255);
    speedRight = map(yAxis, 510, 0, 0, 255);
  }
  // Pivot Left turn
  if (yAxis > 470 && yAxis < 550 && xAxis < 470) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    speedLeft  = map(xAxis, 510, 0, 0, 255);
    speedRight = map(xAxis, 510, 0, 0, 255);
  }
  // Left turn
  else if (xAxis < 470) {
    int xMapped = map(xAxis, 510, 0, 0, 255);
    speedLeft  = speedLeft  - xMapped;
    speedRight = speedRight + xMapped;
    if (speedLeft < 0) speedLeft = 0;
    if (speedRight > 255) speedRight = 255;
  }
  // Pivot Right turn
  if (yAxis > 470 && yAxis < 550 && xAxis > 550) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    speedLeft  = map(xAxis, 510, 1023, 0, 255);
    speedRight = map(xAxis, 510, 1023, 0, 255);
  }
  // Right turn
  else if (xAxis > 550) {
    int xMapped = map(xAxis, 510, 1023, 0, 255);
    speedLeft  = speedLeft  + xMapped;
    speedRight = speedRight - xMapped;
    if (speedLeft > 255) speedLeft = 255;
    if (speedRight < 0) speedRight = 0;
  }
  // Joystick neutral
  if ((yAxis > 470 && yAxis < 550) && (xAxis > 470 && xAxis < 550)) {
    speedLeft  = 0;
    speedRight = 0;
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
  }
  // joystick button LED
  if (buttonK == 1) {
    digitalWrite(LEDR, HIGH);
  }
  else {
    digitalWrite(LEDR, LOW);
  }
  // Motor drive LED
  if(speedLeft > 0 || speedRight > 0) {
    digitalWrite(LEDG, HIGH);
  }else{
    digitalWrite(LEDG, LOW);
  }
  // Motor drive speed
  analogWrite(enA, speedLeft);
  analogWrite(enB, speedRight);
  // serial monitor
  Serial.print(F("\r\nxAxis: "));
  Serial.print(xAxis);
  Serial.print(F("\tyAxis: "));
  Serial.print(yAxis);
  Serial.print(F("\tspeedLeft: "));
  Serial.print(speedLeft);
  Serial.print(F("\tspeedRight: "));
  Serial.print(speedRight);
  Serial.print(F("\tbuttonK: "));
  Serial.print(buttonK);
}

void getReceivePackage() {

}
