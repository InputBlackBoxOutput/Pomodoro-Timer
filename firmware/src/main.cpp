#include <Arduino.h>
#include "connections.h"

unsigned long start_time = 0;
unsigned long now = 0;
bool timer_on = false;

const uint8_t mapping[10] = {
  /*0*/ 0b11111100,
  /*1*/ 0b01100000,
  /*2*/ 0b11011010,
  /*3*/ 0b11110010,
  /*4*/ 0b01100110,
  /*5*/ 0b10110110,
  /*6*/ 0b10111110,
  /*7*/ 0b11100000,
  /*8*/ 0b11111110,
  /*9*/ 0b11110110,
};


void blinkLED(uint8_t times) {
  for(int i=0; i<times; i++) {
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
  }
}

void soundBuzzer(uint8_t times) {
  for(int i=0; i<times; i++) {
    tone(BUZZER, 1000);
    delay(100);    
    noTone(BUZZER);  
    delay(100);
  }
}


int* convertToMinutesSeconds(unsigned long t) {
  static int n[4] = {0, 0, 0, 0};

  t = t / 1000;
  long minutes = t / 60;
  long seconds = t % 60;

  n[1] = (int) (minutes%10);
  if (minutes > 9) {
    n[0] = minutes/10;
  }

  n[3] = (int) (seconds%10);
  if (seconds > 9) {
    n[2] = seconds/10;
  }

  return n;
}

void clearDisplay() {
  digitalWrite(LATCH, LOW);
  for(uint8_t i=0; i<4; i++) {
   shiftOut(SERIAL_DATA, SERIAL_CLK, LSBFIRST, 0);
  }
  digitalWrite(LATCH, HIGH);
}

void writeDigitsOnDisplay(int* n) {
  digitalWrite(LATCH, LOW);
  for(uint8_t i=0; i<4; i++) {
   shiftOut(SERIAL_DATA, SERIAL_CLK, LSBFIRST, mapping[*(n+i)]);
  }
  digitalWrite(LATCH, HIGH);
}

void setup() {
  Serial.begin(115200);

  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  
  pinMode(BTN1, INPUT);
  pinMode(BTN2, INPUT);

  pinMode(SERIAL_DATA, OUTPUT);
  pinMode(SERIAL_CLK, OUTPUT);
  pinMode(LATCH, OUTPUT);

  pinMode(LED_COLON, OUTPUT);
  digitalWrite(LED_COLON, HIGH);

  clearDisplay();
}

void loop() {
  if(digitalRead(BTN1)) {
    soundBuzzer(1);
    blinkLED(1);
    timer_on = true;
    start_time = 0;
  }
  if(digitalRead(BTN2)) {
    soundBuzzer(1);
    blinkLED(1);
    timer_on = false;
    clearDisplay();
  }

  
  if(timer_on) {
    now = millis();

    if(now - start_time == 1500000L) { // 25 minutes
      soundBuzzer(1);
      blinkLED(1);
    }
    if(now - start_time == 1800000L) { // 30 minutes
      soundBuzzer(2);
      blinkLED(2);
    }

    writeDigitsOnDisplay(convertToMinutesSeconds(now));
  }

  delayMicroseconds(100);
}