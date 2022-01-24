#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd (0x27, 16, 2);

const int analogGap = 800;
int next_step;
String currentRow1, currentRow2;

#define pinTrig1 2
#define pinEcho1 3
#define pinTrig2 4
#define pinEcho2 5

#define pinPote A0
int pinsReceivers[3] = {A1, A2, A3};

void setup() {
  Serial.begin(9600);
  initialize_ultrasonic(pinTrig1, pinEcho1);
  initialize_ultrasonic(pinTrig2, pinEcho2);
  for (int k=0; k<3; k++) { pinMode(pinsReceivers[k], INPUT); } 
  Wire.begin(); lcd.begin(16, 2);
  lcd.clear(); lcd.backlight();
}

void loop() {
    
  int d1 = object_detect (1, false);
  int d2 = object_detect (2, false);
  
  String msg = "su1: " + String (d1) + " su2: " + String (d2);
  
  int received[3]; 
  for (int z = 0; z < 3;z++){
     received[z] = Gap(analogRead(pinsReceivers[z]));
     msg += " " +String(pinsReceivers[z]) + ": " + String(Gap(analogRead(pinsReceivers[z])));   
  }
  next_step = getInfo (received);

  switch (next_step){
      case 0: Show ("Seguime", ""); break; 
      case 1: Show ("Seguime", ""); break; 
      case 2: Show ("Seguime", ""); break; 
      case 3: Show ("Seguime", ""); break; 
      case 4: Show ("Seguime", ""); break; 
      case 5: Show ("Automático", ""); break;
      case 6: Show ("Manual", ""); break; 
      case 7: Show ("Calibrar LDR", ""); break;
      case -1: Show("Sin datos", ""); break; 
  }
  
  Serial.println(msg);
}

void Show (String row1, String row2){
 if (currentRow1 != row1){
    lcd.clear();
    lcd.setCursor (0,0); lcd.print(row1); lcd.setCursor (0,1); lcd.print(row2);
    currentRow1 = row1; currentRow2 = row2;     
  } 
}

unsigned int getInfo (int received[3]){
  if (received[0] == 0 & received[1] == 0 & received[2] == 0) { return 0; } // Frenar
  else if (received[0] == 0 & received[1] == 1 & received[2] == 0) { return 1; } // Avanzar
  else if (received[0] == 1 & received[1] == 0 & received[2] == 0) { return 2; } // Izquierda
  else if (received[0] == 0 & received[1] == 0 & received[2] == 1) { return 3; } // Derecha
  else if (received[0] == 1 & received[1] == 1 & received[2] == 1) { return 4; } // Atrás
  else if (received[0] == 1 & received[1] == 1 & received[2] == 0) { return 5; } // Automático
  else if (received[0] == 0 & received[1] == 1 & received[2] == 1) { return 6; } // Manual
  else if (received[0] == 1 & received[1] == 0 & received[2] == 1) { return 7; } // Config. LDR
  else {return -1;}
}

void initialize_ultrasonic (int trig, int echo){
  pinMode(trig, OUTPUT); digitalWrite(trig, LOW); pinMode(echo, INPUT);    
}

int object_detect (int nSensor, bool verb){
    long duracion, distancia; int trig, echo;
    if (nSensor == 1) {trig = pinTrig1; echo = pinEcho1;}
    else {trig = pinTrig2; echo = pinEcho2;}
    digitalWrite(trig, LOW); delayMicroseconds(2);           
    digitalWrite(trig, HIGH); delayMicroseconds(10);     
    digitalWrite(trig, LOW);   
    duracion = pulseIn(echo, HIGH); distancia = duracion / 2 / 29.1;
    if (verb){Serial.println(String(nSensor) + " " + String(distancia) + " cm.");}
    delay (50); return distancia;
}

int Gap (int value){
  if (value > analogGap) { return 1; }
  else { return 0; }
}
