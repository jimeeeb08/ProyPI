#include <LiquidCrystal.h>

// sensibilidad baja ya que sensores detectan del 1-8 aprox en pruebas
const int sens1 = 2;   
const int sens2 = 2;   
const int lecturas = 150;

LiquidCrystal lcd(13, 12, 5, 4, 3, 2); // pines del LCD

const int sensor1 = A0;
const int sensor2 = A1;
const int LED1 = 7;
const int LED2 = 6;
const int BTN  = 8;
// const int FAN  = 10; // ventilador no conectado todavía

int modo = 0; // 0=LED, 1=VENT

int medirNivel(int pin) {
  int vMax = 0;
  int vMin = 1023;
  for (int i = 0; i < lecturas; i++) {
    int v = analogRead(pin);
    if (v > vMax) vMax = v;
    if (v < vMin) vMin = v;
  }
  return (vMax - vMin);
}

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  // pinMode(FAN, OUTPUT); // ventilador no usado aún
  pinMode(BTN, INPUT_PULLUP);

  lcd.begin(16, 2);
}

void loop() {
  // cambio de modo con el botón
  if (digitalRead(BTN) == LOW) {
    modo = 1 - modo;  // alterna entre LED y VENT
    delay(200);       
    while (digitalRead(BTN) == LOW); 
  }

  int nivel1 = medirNivel(sensor1);
  int nivel2 = medirNivel(sensor2);
  
  bool led1On = (nivel1 > sens1);
  bool led2On = (nivel2 > sens2);
  

  if (modo==1){
     digitalWrite(LED1, LOW);

  }
  else if(modo==0 ){
     digitalWrite(LED2, LOW);
  }
  if (modo == 0 && led1On) {   // Sensor 1 
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, LOW);
    
    
  } else if (modo == 1 && led2On) {           // Sensor 2 
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, HIGH);
  
  }



// LCD
// Primera fila: Sensor 1 y LED 1
lcd.setCursor(0, 0);  // fila 0, columna 0
if (modo == 0 && led1On) {
  lcd.print("S1:ON LED1:ON   "); // espacios al final para limpiar
} else if(modo == 0 && !led1On) {
  lcd.print("S1:ON LED1:OFF     ");

}
else{
  lcd.print("S1:OFF LED1:OFF     ");

}

// Segunda fila: Sensor 2 y LED 2
lcd.setCursor(0, 1);  // fila 1, columna 0
if (modo == 1 && led2On) {
  lcd.print("S2:ON LED2:ON   "); // espacios al final para limpiar
} else if(modo == 1 && !led2On) {
  lcd.print("S2:ON LED2:OFF     ");

}
else{
  lcd.print("S2:OFF LED2:OFF     ");

}

 
  delay(200); 
}