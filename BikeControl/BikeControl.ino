const int TICKS_DELAY = 50;
const int WELCOME_TICKS = 50;

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd (0x27, 16, 2);

#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

#define pote A1
#define button 3
#define led 13

// #define forn (MAX) for (int q=0; q<MAX; q++)

#define keyLetter '$' // $message$

const String PRESS_BT_CHR = "Pulsa el boton para iniciar ";
int chrDisplayCounter = 0;

int mode; int cMode = 1; 
String coms_msg;
int laps, pLaps = 0;
String actualTime = "No hay datos";
String actualSpeed = "0 km/h";
String goalDistance = "No hay datos";
String traveledDistance = "0 km.";
String avgSpeed = "0 km/h"; 
double remainingDistance; 

const int qChron = 15; int chrCounter = 0; bool chrStarted = false; 
int chrTime[3];

void setup() {
  Serial.begin (9600);
  pinMode (pote, INPUT);
  pinMode (led, OUTPUT);
  
  Wire.begin(); lcd.begin(16, 2);
  lcd.clear(); lcd.backlight();

  dht.begin();
    
  lcd.setCursor(0,0); lcd.print("Bienvenido");
  for (int a=0; a<17; a++) {  progressBar(a); delay(WELCOME_TICKS); }
  lcd.clear();
}

void progressBar (int n) {
  for (int u=0; u<n; u++){
    lcd.setCursor(u, 1);
    lcd.print (".");
  }
}

String readLine () {
  String acc = ""; 
  if ((char)Serial.read() == keyLetter) {
      while ((char)Serial.read() != keyLetter) { acc += Serial.read(); }
  }
  return acc;
}

void Display (String upper, String lower){
  lcd.setCursor(0,0); lcd.print(upper);
  lcd.setCursor(0, 1); lcd.print(lower);

  if (upper.length() > 16 or lower.length() > 16){
    lcd.scrollDisplayLeft();
    delay(250);
  }
}

void updateData (String data) {
    actualTime = ""; for (int q=0; q<8; q++) { actualTime += data[q]; } 
    actualSpeed = ""; for (int q=0; q<2; q++) { actualSpeed += data[8+q]; } // km per hour
    goalDistance = ""; for (int q=0; q<2; q++) { goalDistance += data[10+q]; } // km 
    traveledDistance = ""; for (int q=0; q<2; q++) { traveledDistance += data[12+q]; } // km
    remainingDistance = goalDistance.toFloat () - traveledDistance.toFloat (); // km
    avgSpeed = ""; for (int q=0; q<3; q++) { avgSpeed += data[14+q]; }
}

void displayTime (int Time[]){

  lcd.setCursor(6,1); lcd.print(":");
  lcd.setCursor(9,1); lcd.print(":");
  
  if (Time[0] < 10){
    lcd.setCursor(4, 1); lcd.print("0");
    lcd.setCursor(5, 1); lcd.print(Time[0]);
  } else {
    lcd.setCursor(4, 1); lcd.print(Time[0]);
  }

  if (Time[1] < 10){
    lcd.setCursor(7, 1); lcd.print("0");
    lcd.setCursor(8, 1); lcd.print(Time[1]);
  } else {
    lcd.setCursor(7, 1); lcd.print(Time[1]);
  }

  if (Time[2] < 10){
    lcd.setCursor(10, 1); lcd.print("0");
    lcd.setCursor(11, 1); lcd.print(Time[2]);
  } else {
    lcd.setCursor(10, 1); lcd.print(Time[2]);
  }
}

void restartChr () {
  for (int i=0; i<3; i++) { chrTime[i] = 0; }
  chrCounter = 0; pLaps = 0; chrDisplayCounter = 0;
  lcd.clear(); lcd.setCursor (0,0); lcd.print(" Cronometro");
}

void updateChr () {
  
  chrTime[0] = int (chrCounter / 3600);
  chrTime[1] = int ((chrCounter - chrTime[0] * 3600) / 60);
  chrTime[2] = int ((chrCounter - chrTime[0] * 3600) % 60);
  
  Serial.print(chrTime[0]); Serial.print(" ");
  Serial.print(chrTime[1]); Serial.print(" ");
  Serial.print(chrTime[2]); Serial.println();
}

void loop() {
    
  coms_msg = readLine ();
  if (coms_msg.length() > 0) { updateData (coms_msg); laps++; }     
  mode = map (analogRead (pote), 0, 1010, 1, 7);
  
  switch (mode) {
    case 1: // Display Time 
      if (cMode != 1){
        lcd.clear();
      }
      lcd.setCursor (0,0); lcd.print("    Horario ");
      lcd.setCursor (0,1);
      if (actualTime.length()<12){lcd.print ("    "+actualTime);}
      else {lcd.print("  "+actualTime);}
      cMode = 1;
      break;
    case 2: // Display Chronometre 
      if (cMode != 2) {
        lcd.clear(); lcd.setCursor (0,0); lcd.print(" Cronometro");
      }
      if (digitalRead(button) == 1) { chrStarted = !chrStarted; restartChr(); digitalWrite (led, chrStarted ? 1:0); }
      if (chrStarted) {
        updateChr(); displayTime(chrTime);        
      } else {
        lcd.setCursor(0,1);
        String buff = "";
        for (int z=chrDisplayCounter; z<chrDisplayCounter+16; z++){
          buff += PRESS_BT_CHR[z % PRESS_BT_CHR.length()];
        }
        lcd.print(buff);
        delay(250);      
        chrDisplayCounter++;
      } 
      cMode = 2;
      break;
    case 3: // Display Distance Done 
      if (cMode != 3){ lcd.clear(); lcd.home(); lcd.print(" Distancia Rec.");}
      lcd.setCursor(0,1); lcd.print("     "+traveledDistance);
      cMode = 3;
      break;
    case 4: // Display Speed
      if (cMode != 4) {
        lcd.clear(); lcd.setCursor (0,0); lcd.print(" Velocidad");
      }
      lcd.setCursor(0,1); lcd.print ("    "+ actualSpeed);
      cMode = 4;
      break;
    case 5: // Display average speed
      if (cMode != 5) {
        lcd.clear(); lcd.setCursor (0,0); lcd.print("Velocidad Media");
      }
      lcd.setCursor(0,1); lcd.print ("    " + avgSpeed);
      cMode = 5;
      break;
    case 6:
      if (cMode != 6) {lcd.clear();}
      lcd.setCursor (0,0); lcd.print ("-> Temp (C): " + String (dht.readTemperature()));
      lcd.setCursor (0,1); lcd.print ("-> Hum (%): " + String (dht.readHumidity()));
      cMode = 6;
      break;
  }

   if (chrStarted) {
    if (pLaps % qChron == 0) { chrCounter++; pLaps = 0; }
    pLaps++;
  } 

  /*
    Serial.print(mode); Serial.print(" ");
    Serial.print(chrStarted?"Y":"N"); Serial.print(" ");
    Serial.print(chrCounter); Serial.print(" ");
    Serial.print(pLaps); Serial.print(" ");
    Serial.println();
  */

  delay(TICKS_DELAY);
}
