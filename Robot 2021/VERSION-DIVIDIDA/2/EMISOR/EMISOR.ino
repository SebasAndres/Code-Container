int gapLDR; // Si el ldr_n es menor a gapLDR, entonces bit_n = 1

#define TRANSMITTER1 A1
#define TRANSMITTER2 A2
#define TRANSMITTER3 A3

#define pinLDR A4
int pinsLDR[3] = {A4, A4, A4};
int pinsLeds[3] = {12, 12, 12};

#define pote A7

int x, y, z;

void setup() {
  Serial.begin(9600);
  pinMode (TRANSMITTER1, OUTPUT);
  pinMode (TRANSMITTER2, OUTPUT);
  pinMode (TRANSMITTER3, OUTPUT);
  for (int ldr=0; ldr<sizeof(pinsLDR)/sizeof(pinsLDR[0]); ldr++) { pinMode (pinsLDR[ldr], INPUT);}
  for (int led=0; led<sizeof(pinsLeds)/sizeof(pinsLeds[0]); led++) { pinMode (pinsLeds[led], INPUT);}
  gapLDR = analogRead(pinLDR) - 5;
  pinMode (pote, INPUT);
  pinMode (13, OUTPUT);
}

void loop() {
  if (analogRead(pote) > 868){ x = 1; y = 1; z= 0; } // Automático
  else if (analogRead(pote) > 612){ // Modo seguir
    x = Gap(analogRead(pinsLDR[0])); y = Gap(analogRead(pinsLDR[1])); z = Gap(analogRead(pinsLDR[2]));    
    for (int ldr=0; ldr<3; ldr++) { 
      if (Gap (analogRead(pinsLDR[ldr])) == 1) { digitalWrite (pinsLeds[ldr], HIGH); }
      else { digitalWrite (pinsLeds[ldr], LOW); }
    }
  }
  else if (analogRead(pote) > 476) { x = 1; y = 0; z = 1; } // Modo configurar
  else { x = 0; y = 1; z = 1; } // Manual  
  send_byte (x, y, z);
}

int Gap (int val){
  if (val < gapLDR) { return 1; }
  else { return 0; }  
}

void send_byte (int x, int y, int z){
  Serial.println ("Enviando "+String(x)+"  "+String(y)+"  "+String(z));
  digitalWrite (TRANSMITTER1, x);  digitalWrite (TRANSMITTER2, y); digitalWrite (TRANSMITTER3, z);
}
