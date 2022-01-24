// Preferencia
const int LONG_BEEP = 4;
const int RELEASE_BEEP = 100;
const int TICK_DELAY = 50;

// Time 
int nowTime[] = {0, 0};
int Set[] = {0, 0};
double time_passed = 0.0;
double time_helper = 0.00;

// Temp y Hum
double temp_state[] = {0, 0};

// Prueba:  int horarios_alarmas[3][2] = {{0, 34}, {1, 31}, {0, 29}};
int horarios_alarmas[3][2] = {{77,77}, {88,88}, {99,99}};
int n_alr = 0;

// Componentes
const int led_horario = 7;
const int led_error = 13;
const int dht_pin = 2;
const int boton = 12; 
const int pote_mode = A1;
const int pote_value = A0;

int hora_pantalla, minutos_pantalla;
int cr_hora_elegida, cr_minutos_elegidos, cr_segundos_elegidos, cr_state_controller;

// Notas
float nota = 0;

float C = 32.7;
float Db = 34.65;
float D = 36.71;
float Eb = 38.89;
float E = 41.2;
float F = 43.65;
float Gb = 46.25;
float G = 49.00;
float Ab = 25.96;
float A = 55.0;
float Bb = 58.25;
float B = 61.74;

bool stop_alert = false;

#include <ThreeWire.h>  
#include <RtcDS1302.h>

ThreeWire myWire(4,5,2); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd (0x27, 16, 2);
int MODE = 0; // 0 : proxima alarma // 1 : ver alarmas // 2 : añadir alarma

#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(57600);  
  
  dht.begin();
  Rtc.Begin();
  
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  if (compiled.IsValid()){
    Serial.println("<Compilación correcta de RTC>");
  } else {
    Serial.println("<Compilación incorrecta de RTC>");  
  }
  Serial.print("> Hora inicio: Año ");
  Serial.print(compiled.Year()); Serial.print(" || Mes ");
  Serial.print(compiled.Month()); Serial.print(" || Día ");
  Serial.print(compiled.Day()); Serial.print(" || Hora ");
  Serial.print(compiled.Hour()); Serial.print(" || Minutos ");
  Serial.print(compiled.Minute()); 
  Serial.println();
  
  pinMode (led_error, OUTPUT);
  pinMode (led_horario, OUTPUT);

//  pinMode (motor, OUTPUT);
  
  pinMode(pote_mode, INPUT);
  pinMode(pote_value, INPUT);

  pinMode(boton, INPUT);
  
  beep();
  
  Wire.begin();
  lcd.begin(16, 2);
  lcd.clear();
  lcd.backlight();

  UpdateLastAlarm();
  
}

int dif(int horario[]) {
  RtcDateTime now = Rtc.GetDateTime();
  int f = 60 * (horario[0] - now.Hour()) + horario[1] - now.Minute();
  if (f > 0){
    return f;
  }
  else{
    return 24*60 - f;
  }
}

void UpdateLastAlarm(){

  if (MODE != 0){
    lcd.clear();
  }

  lcd.setCursor(0, 0);
  lcd.print(" Proxima alarma");
    
  //int Size = sizeof(horarios_alarmas) / sizeof(horarios_alarmas[0]);
  int Size = n_alr;
  if (Size > 0){
    int MinDif = dif(horarios_alarmas[0]);   
    int index_MinDif = 0;
    for (int j = 0; j < Size; j++){
      if (MinDif > dif(horarios_alarmas[j])){
        MinDif = dif(horarios_alarmas[j]);
        index_MinDif = j;
      }
    }
    
    MostrarHorario(horarios_alarmas[index_MinDif][0], horarios_alarmas[index_MinDif][1]);
  } else {
    lcd.setCursor(1, 1);
    lcd.print ("No hay alarmas");    
  }
}

bool _contains_ (int lista_2d[3][2], int lista[]){ // lista_2d[1][2]
  bool retorno = false;
  int sizeOf = sizeof(lista_2d)/sizeof(lista_2d[0]);
  for (int j = 0 ; j <= sizeOf ; j++){  
     if (lista_2d[j][0] == lista[0] & lista_2d[j][1] == lista[1]){
        retorno = true;
     } 
  }
  return retorno;
}

void beep(){  
  // G - C - D# - F (4x) || G - C - E - F (4x)
 int tiempos[] = {200, 200, 100, 100}; 
 float acorde1[] = {G, C, Eb, F};
 float acorde2[] = {G, C, E, F};
 for (int i=0; i < LONG_BEEP; i++){  
   for (int nota=0; nota < 4; nota++){
     tone(11, 8 * acorde1[nota], RELEASE_BEEP);
     delay(tiempos[nota]); 
   }
 }
 for (int i=0; i<LONG_BEEP; i++){
   for (int nota=0; nota < 4; nota++){
     tone(11, 8 * acorde2[nota], RELEASE_BEEP);
     delay(tiempos[nota]); 
   }  
 }
}

void VerAlarmas(){
  //int Size = sizeof(horarios_alarmas) / sizeof(horarios_alarmas[0]);
  int Size = n_alr;
  if (MODE != 1){
    lcd.clear();  
  }
  lcd.setCursor(0,0);
  lcd.print("Alarmas.    (");
  lcd.setCursor(14, 0);
  lcd.print(n_alr);
  lcd.setCursor(15, 0);
  lcd.print(")");

  int delta = 1010 / Size;
  for (int i = 0; i < Size; i++){
    if (analogRead(pote_value) > (Size - i) * delta & analogRead(pote_value) < (Size - i + 1) * delta){
      MostrarHorario(horarios_alarmas[i][0], horarios_alarmas[i][1]);
    } 
  }
  
}

void MostrarHorario(int hora, int minutos){
  if (hora_pantalla != hora & minutos_pantalla != minutos){
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }
 
  lcd.setCursor(7,1);
  lcd.print(":");
  if (hora < 10){
    lcd.setCursor(5, 1);
    lcd.print("0");
    lcd.setCursor(6, 1);
    lcd.print(hora);
  } else { 
    lcd.setCursor(5, 1);
    lcd.print(hora);
  }
  if (minutos < 10){
    lcd.setCursor(8, 1);
    lcd.print("0");
    lcd.setCursor(9, 1);
    lcd.print(minutos);
  } else {
    lcd.setCursor(8,1);
    lcd.print(minutos);
  }
  hora_pantalla = hora;
  minutos_pantalla = minutos;
}

void MostrarHorarioSegs(int hora, int minutos, int segundos){
  if (hora_pantalla != hora & minutos_pantalla != minutos){
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }
  
  lcd.setCursor(6,1);
  lcd.print(":");
  lcd.setCursor(9,1);
  lcd.print(":");
  
  if (hora < 10){
    lcd.setCursor(4, 1);
    lcd.print("0");
    lcd.setCursor(5, 1);
    lcd.print(hora);
  } else{
    lcd.setCursor(4, 1);
    lcd.print(hora);
  }
  if (minutos < 10){
    lcd.setCursor(7, 1);
    lcd.print("0");
    lcd.setCursor(8, 1);
    lcd.print(minutos);
  } else{
    lcd.setCursor(7,1);
    lcd.print(minutos);
  }
  if (segundos < 10){
    lcd.setCursor(10, 1);
    lcd.print("0");
    lcd.setCursor(11, 1);
    lcd.print(segundos);
  } else{ 
    lcd.setCursor(10, 1);
    lcd.print(segundos);
  }
  
  hora_pantalla = hora;
  minutos_pantalla = minutos;
}

bool in_range (int num, int _min_, int _max_){
  bool ret = false;
  if (num <= _max_ & num >= _min_){
    ret = true;
  }
  return ret;
}

void Cronometro(){
  
  if (MODE != 4){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(" < Cronometro > ");
    MostrarHorario(0,0);
    cr_state_controller = 1; 
    cr_hora_elegida = 0;
    cr_minutos_elegidos = 0;
    cr_segundos_elegidos = 0;
    Serial.println("MODE != 4");
  }
  
  if (digitalRead(boton) == 0){
    delay(10);
    if (cr_state_controller == 1){
      cr_state_controller = 2;
    } else if (cr_state_controller == 2 & digitalRead(boton) != 0){
      cr_state_controller = 3;
    } 
    else if (cr_state_controller == 3 & digitalRead(boton) != 0){
      cr_state_controller = 4;
    }
  }

  if (cr_state_controller == 2){ // min 
    cr_minutos_elegidos = map (analogRead(pote_value), 0, 1010, 59, 0);
  } else if (cr_state_controller == 3){ // hor
    cr_hora_elegida = map (analogRead(pote_value), 0, 1010, 24, 0);
  } else if (cr_state_controller == 1){ // seg
    cr_segundos_elegidos = map (analogRead(pote_value), 0, 1010, 59, 0);
  } else { // Cronómetro
    lcd.setCursor(0, 0);
    lcd.print("Tiempo restante");
    
    int tiempo_espera = 60 * cr_minutos_elegidos + 3600 * cr_hora_elegida + cr_segundos_elegidos;
    int TiempoInicial = tiempo_espera;
    
    int horas_restantes = cr_hora_elegida;
    int minutos_restantes = cr_minutos_elegidos;
    int segundos_restantes = cr_segundos_elegidos;

    int tiempo_acum = 0;
    while (in_range(analogRead(pote_mode), 300, 500) & tiempo_acum != TiempoInicial){
      delay(1000);
      time_passed += 1.0;
      
      tiempo_espera -= 1;
      
      horas_restantes =  tiempo_espera / 3600;
      minutos_restantes = (tiempo_espera - (horas_restantes * 3600)) / 60;
      segundos_restantes = tiempo_espera - horas_restantes * 3600 - minutos_restantes * 60;
      
      MostrarHorarioSegs(horas_restantes, minutos_restantes, segundos_restantes);
      tiempo_acum += 1;

      Serial.print("<Cronómetro>: Te faltan ");
      Serial.print(tiempo_espera);
      Serial.println();
    }

    if (tiempo_acum == TiempoInicial){
//      digitalWrite(motor, HIGH);
      digitalWrite(led_horario, HIGH);
      while (in_range(analogRead(pote_mode), 300, 500)){  
        beep();
      }
      // digitalWrite(motor, LOW);
      digitalWrite(led_horario, LOW);
    } 
  }
 
  if (cr_state_controller != 4)
    MostrarHorarioSegs(cr_hora_elegida, cr_minutos_elegidos, cr_segundos_elegidos);
}

void AddAlarm (){
  
  if (MODE != 2){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("<Añadir Alarma>");
    MostrarHorario(0,0);
    cr_state_controller = 1; 
    cr_hora_elegida = 0;
    cr_minutos_elegidos = 0;
  }
  
  if (digitalRead(boton) == 0){
    delay(10);
    
    time_passed += 0.01;
    
    if (cr_state_controller == 1){
      cr_state_controller = 2;
    } else if (cr_state_controller == 2 & digitalRead(boton) != 0){
      cr_state_controller = 3;
    } else {
      cr_state_controller = 4;  
    }
  }

  if (cr_state_controller == 1){
    cr_hora_elegida = map (analogRead(pote_value), 0, 1010, 24, 0);
  } else if (cr_state_controller == 2){
    cr_minutos_elegidos = map (analogRead(pote_value), 0, 1010, 59, 0);
  } else {
      // Averigua si la alarma ya está registrada
      lcd.setCursor(0,1);
      lcd.print("                ");
      
      int nueva_alarma[] = {cr_hora_elegida, cr_minutos_elegidos};
      bool EXISTE = false;
      for (int i = 0; i < n_alr; i++){
        if (horarios_alarmas[i][0] == nueva_alarma[0] & horarios_alarmas[i][1] == nueva_alarma[1]){
          EXISTE = true;
        }
      }
      
      lcd.setCursor(0,1);
      if (!EXISTE) {   
        horarios_alarmas[n_alr][0] = cr_hora_elegida;
        horarios_alarmas[n_alr][1] = cr_minutos_elegidos;
        n_alr ++;
        lcd.print("Alarma agregada");
      } else {
        lcd.setCursor(3, 1);
        lcd.print("Ya existe");
      }
      delay(300);
      time_passed += 0.3;
  }

  if (cr_state_controller < 3)
    MostrarHorario(cr_hora_elegida, cr_minutos_elegidos);
}

void read_dht(){
  temp_state[0] = dht.readTemperature();
  temp_state[1] = dht.readHumidity();
}

void _setTime_(){

  if (MODE != 3){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(" < Fijar hora > ");
    MostrarHorario(nowTime[0],nowTime[1]);
    cr_state_controller = 1; 
    cr_hora_elegida = 0;
    cr_minutos_elegidos = 0;
  }

  lcd.setCursor(0,0);
  lcd.print(" < Fijar hora > ");
  MostrarHorario(cr_hora_elegida,cr_minutos_elegidos);
  
  if (digitalRead(boton) == 0){
    delay(10);
    if (cr_state_controller == 1){
      cr_state_controller = 2;
    } else if (cr_state_controller == 2 & digitalRead(boton) != 0){
      cr_state_controller = 3;
    } else {
      cr_state_controller = 4;  
    }
  }
  
  if (cr_state_controller == 1){
    cr_hora_elegida = map (analogRead(pote_value), 0, 1010, 24, 0);
  } else if (cr_state_controller == 2){
    cr_minutos_elegidos = map (analogRead(pote_value), 0, 1010, 59, 0);
  } else {
    // Fijar horario -> {cr_hora_elegida, cr_minutos_elegidos}   
    nowTime[0] = cr_hora_elegida;
    nowTime[1] = cr_minutos_elegidos;

    Set[0] = cr_hora_elegida;
    Set[1] = cr_minutos_elegidos;
    
    lcd.setCursor(0,1);
    lcd.print (" Horario fijado");
    delay(100);

    time_passed += 0.01;
    
    lcd.clear();
  
    cr_state_controller = 1;
  } 
}

void adapt (){
  while (nowTime[0] >= 24){
      nowTime[0] -= 24;
  }
  while (nowTime[0] >= 59){
      nowTime[0] -= 59;
  }
}

void updateTime (double time_passsed){

  float dd = int (time_passed / 60);
  int dif;

  if (dd > 0){
    while (dd != 0){  
       dif = int (dd / 60);    
       if (dif < 60){
          nowTime[1] = Set[1] + dd;
          dd -= dd;
       } else {
          nowTime[0] = Set [0] + int (dif / 60);
          nowTime[1] = Set [1] + time_passed - dif * 60;
          dd -= time_passed - dif * 60;
       }
       Serial.print ("En loop <Upt Time> : ");
       Serial.print (nowTime[0]);
       Serial.print (":");
       Serial.print (nowTime[1]);
       Serial.print (" ; dd : ");
       Serial.println (dd); 
  
    }
  }

  // adapt (); // adaptamos hora actual a rango 24:59 
}

void TyH(){
  
  lcd.clear();
  lcd.setCursor (0,0);

  String temp_msj = "-> Temp (C): " + String (temp_state[0]);   
  lcd.print (temp_msj);
  lcd.setCursor (0,1);
  
  String hum_msj = "-> Hum (%): " + String (temp_state[1]);   
  lcd.print (hum_msj);
  
}

void loop() {

  read_dht(); // [0] : Tº y [1] : % Hum
  
  if (temp_state[0] > 48){
    digitalWrite (led_error, HIGH);
    beep();
  }
  
  // int ahora[] =  {now.Hour(), now.Minute()};
  bool enAlarma = _contains_(horarios_alarmas, nowTime);
  if (enAlarma){   
    digitalWrite (led_horario, HIGH);
    beep();
    
     if (!stop_alert)
      // digitalWrite(motor, HIGH);
    if (digitalRead(boton)==1){
      stop_alert = true;
    } 
     
  } else {
    digitalWrite (led_horario, LOW);
    stop_alert = false;
  }

  if (analogRead(pote_mode) >= 900){ // Modo próxima alarma
    TyH();
    MODE = 0;
  } else if (analogRead(pote_mode) >= 850){ // Modo ver alarmas
    VerAlarmas();
    MODE = 1;
  } else if (analogRead(pote_mode) >= 700){ // Modo añadir alarma
    AddAlarm();
    MODE = 2;
  } else if (analogRead(pote_mode) >= 550){
    _setTime_ (); // agregar nueva hora
    MODE = 3;
  } else if (analogRead(pote_mode) >= 300){
    Cronometro();
    MODE = 4;
  } else {
    if (MODE != 5){
      lcd.clear();
      lcd.setCursor (0,0);
      lcd.print ("  < Horario > ");
      MostrarHorario (int (nowTime[0]), int (nowTime[1]));
    }
    MODE = 5;  
  }
   
  Serial.print("--> pote_mode: ");
  Serial.print(analogRead(pote_mode));
  Serial.print(" || pote_value: ");
  Serial.print(analogRead(pote_value));
  Serial.print(" || Button: ");
  Serial.print(digitalRead(boton) ? 'N':'P');
  Serial.print(" || cr_state_controller: ");
  Serial.print(cr_state_controller);
  Serial.print(" || cr_hora: ");
  Serial.print(cr_hora_elegida);
  Serial.print(" || cr_min: ");
  Serial.print(cr_minutos_elegidos);
  Serial.print(" || n_alr: ");
  Serial.print(n_alr);
  Serial.print(" || Alr: ");
  Serial.print(enAlarma ? 'S': 'N');
  Serial.print(" || Tp: ");
  Serial.print(time_passed);
  Serial.print(" || Th: ");
  Serial.print(time_helper); 
  Serial.println();

  delay(TICK_DELAY);

  if (time_helper > 100){
    time_passed += 1;
    time_helper -= 100;
  }

  // time_helper += (TICK_DELAY/1000 + 1/14)*100; // (TICK_DELAY/1000 + 1/14) se aproxima a 0,12
  time_helper += 12;
  
  updateTime (time_passed);
}
