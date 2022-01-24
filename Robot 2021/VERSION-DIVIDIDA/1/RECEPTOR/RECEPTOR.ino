// Sensor Ultrasónico 
#define pinTrig1 2
#define pinEcho1 3
#define pinTrig2 4
#define pinEcho2 5

void setup() {
  Serial.begin(9600);
  initialize_ultrasonic(pinTrig1, pinEcho1);
  initialize_ultrasonic(pinTrig2, pinEcho2);
}

void loop() {
  int d1 = object_detect (1, false);
  int d2 = object_detect (2, false);

  Serial.write("t");
}

void initialize_ultrasonic (int trig, int echo){
  pinMode(trig, OUTPUT); 
  digitalWrite(trig, LOW);
  pinMode(echo, INPUT);    
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
