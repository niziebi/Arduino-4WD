// Arduino-4WD 受信機(車)
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// 各種ピンの定数
const int IN_1 = 2;
const int IN_2 = 4;
const int IN_3 = 7;
const int IN_4 = 8;
const int EN_A = 3;
const int EN_B = 5;
const int LEDR = A3;
const int LEDG = A4;

// 通信に必要な定数
const int CHANNEL = 74;
const byte ADDRESS[6] = "00001";

enum Status {
    SUCCESS,
    NOT_CONNECTION
};

enum Mode {
    NORMAL,
    FAST,
    SLOW
};

// 車の移動方向
enum Direction {
    FORWARD,
    BACKWARD,
    RIGHT,
    LEFT,
    NEUTRAL
};

// 左右モーターのスピード
struct Speed {
    int right;
    int left;
};

// 通信データ
struct Data {
    int joystickX;
    int joystickY;
    boolean button;
};

// 通信用オブジェクト
RF24 radio(8, 10);
Data data;

// 車のステータス
Mode mode = NORMAL;
Status status = NOT_CONNECTION;

long lastRun = 0;
boolean count = false;

// 移動方向を取得する
Direction getDirection(Data data) {
    if(data.joystickY > 550) return FORWARD;
    if(data.joystickY < 470) return BACKWARD;
    if(data.joystickX > 550) return RIGHT;
    if(data.joystickX < 470) return LEFT;
    return NEUTRAL;
}

// スピードを取得する
Speed getSpeed(Data data) {
    Direction direction = getDirection(data);
    Speed speed;
    switch(direction) {
        case FORWARD:
            speed = {
                (int)map(data.joystickY, 510, 1023, 0, 255),
                (int)map(data.joystickY, 510, 1023, 0, 255)
            };
            if(data.joystickX > 550) {
                int xMapped = map(data.joystickX, 510, 1023, 0, 255);
                speed.right -= xMapped;
                speed.left += xMapped;
                if(speed.right < 0) speed.right = 0;
                if(speed.left > 255) speed.left = 255;
            }
            if(data.joystickX < 470) {
                int xMapped = map(data.joystickX, 510, 0, 0, 255);
                speed.right += xMapped;
                speed.left -= xMapped;
                if(speed.right > 255) speed.right = 255;
                if(speed.left < 0) speed.left = 0;
            }
            return speed;
        case BACKWARD:
            speed = {
                (int)map(data.joystickY, 510, 0, 0, 255),
                (int)map(data.joystickY, 510, 0, 0, 255)
            };
            if(data.joystickX > 550) {
                int xMapped = map(data.joystickX, 510, 1023, 0, 255);
                speed.right -= xMapped;
                speed.left += xMapped;
                if(speed.right < 0) speed.right = 0;
                if(speed.left > 255) speed.left = 255;
            }
            if(data.joystickX < 470) {
                int xMapped = map(data.joystickX, 510, 0, 0, 255);
                speed.right += xMapped;
                speed.left -= xMapped;
                if(speed.right > 255) speed.right = 255;
                if(speed.left < 0) speed.left = 0;
            }
            return speed;
        case RIGHT: 
            return {
                (int)map(data.joystickX, 510, 1023, 0, 255),
                (int)map(data.joystickX, 510, 1023, 0, 255)
            };
        case LEFT: 
            return {
                (int)map(data.joystickX, 510, 0, 0, 255),
                (int)map(data.joystickX, 510, 0, 0, 255)
            };
        default: return {0, 0};
    }
}

// 車を動かす
void move(Direction direction, Speed speed) {
    digitalWrite(IN_1, direction == FORWARD || direction == RIGHT ? HIGH : LOW);
    digitalWrite(IN_2, direction == BACKWARD || direction == LEFT ? HIGH : LOW);
    digitalWrite(IN_3, direction == FORWARD || direction == LEFT ? HIGH : LOW);
    digitalWrite(IN_4, direction == BACKWARD || direction == RIGHT ? HIGH : LOW);
    analogWrite(EN_B, speed.right);
    analogWrite(EN_A, speed.left);
}

// 初期化
void setup() {
    Serial.begin(9600);
    radio.begin();
    radio.openReadingPipe(0, ADDRESS);
    radio.setAutoAck(false);
    radio.setDataRate(RF24_250KBPS);
    radio.setPALevel(RF24_PA_MAX);
    radio.setChannel(CHANNEL);
    radio.startListening();
    pinMode(EN_A, OUTPUT);
    pinMode(IN_1, OUTPUT);
    pinMode(IN_2, OUTPUT);
    pinMode(IN_3, OUTPUT);
    pinMode(IN_4, OUTPUT);
    pinMode(EN_B, OUTPUT);
    pinMode(LEDR, OUTPUT);
    pinMode(LEDG, OUTPUT);
}

// ループ
void loop() {
    Direction direction = NEUTRAL;
    Speed speed = {0, 0};
    status = radio.available() ? SUCCESS : NOT_CONNECTION;
    if(radio.available()) {
        radio.read(&data, sizeof(Data));
        direction = getDirection(data);
        speed = getSpeed(data);
    }
    move(direction, speed);
    if(millis() - lastRun >= 500) {
        lastRun = millis();
        count = count ? false : true;
        switch(status) {
            case SUCCESS:
                digitalWrite(LEDG, HIGH);
                digitalWrite(LEDR, LOW);
                break;
            case NOT_CONNECTION:
                digitalWrite(LEDG, LOW);
                digitalWrite(LEDR, count ? HIGH : LOW);
                break;
        }
    }
    Serial.print(F("\r\nDirection: "));
    Serial.print(direction);
    Serial.print(F("\tSpeed: [ "));
    Serial.print(F("Right: "));
    Serial.print(speed.right);
    Serial.print(F("\tLeft: "));
    Serial.print(speed.left);
    Serial.print(F(" ]\tLastRun: "));
    Serial.print(lastRun);
}