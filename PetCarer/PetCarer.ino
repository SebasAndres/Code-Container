#include <Servo.h>

#define button_pin 2
#define door_pin 9
#define led_pin 13

const int LOOP_DELAY = 100;

Servo Door;
bool door_state;

void setup() {
  pinMode (button_pin, INPUT);
  pinMode (led_pin, OUTPUT);
  
  Door.attach(door_pin);
  
  Serial.begin(9600);
}

void change_state (){ door_state = !door_state;}

void loop() {

  if (digitalRead(button_pin) == 1) {door_state = !door_state;}
  
  if (door_state){ Door.write(180); }
  else {  Door.write(0); }
  Serial.println(digitalRead(button_pin));

  delay(LOOP_DELAY);
}
