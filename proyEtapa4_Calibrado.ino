#include <LiquidCrystal.h>

// ==== Configuración de sensibilidad ====
int sens1 = 8;   // Se recalibrará automáticamente
int sens2 = 8;   
const int lecturas = 100; // Se reducen cantidad de lecturas para respuesta rápida

LiquidCrystal lcd(13, 12, 5, 4, 3, 2); // Pines del LCD

const int sensor1 = A0;
const int sensor2 = A1;
const int LED1 = 7;
const int LED2 = 6;
const int BTN  = 8;

int modo = 0; // 0 = LED1 controlado por sensor1, 1 = LED2 controlado por sensor2

// Estado actual de LEDs (encendido/apagado)
bool estadoLED1 = false;
bool estadoLED2 = false;

// Evita múltiples lecturas del mismo aplauso
unsigned long ultimaDeteccion1 = 0;
unsigned long ultimaDeteccion2 = 0;
const unsigned long intervaloSilencio = 700;

// --- Función que mide el nivel de variación del micrófono ---
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

// ---  Calibración automática del entorno ---
int calibrar(int pin) {
  long suma = 0;
  for (int i = 0; i < 100; i++) {  // 100 lecturas para promediar el ruido ambiente
    suma += analogRead(pin);
    delay(10);
  }
  int promedio = suma / 100;
  return promedio + 10;  // +10 da un margen de sensibilidad (más = menos sensible)
}

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(BTN, INPUT_PULLUP);
  lcd.begin(16, 2);
  Serial.begin(9600);

  // --- Calibración automática al iniciar ---
  lcd.clear();
  lcd.print("Calibrando...");
  sens1 = calibrar(sensor1);
  sens2 = calibrar(sensor2);

  lcd.clear();
  lcd.print("Listo! S1:");
  lcd.print(sens1);
  lcd.setCursor(0, 1);
  lcd.print("S2:");
  lcd.print(sens2);
  delay(2000); // Muestra valores calibrados 2 segundos
  lcd.clear();

  Serial.print("Umbral sensor1: "); Serial.println(sens1);
  Serial.print("Umbral sensor2: "); Serial.println(sens2);
}

void loop() {
  // --- Cambio de modo con el botón ---
  if (digitalRead(BTN) == LOW) {
    modo = 1 - modo;  // alterna entre LED1 y LED2
    delay(200);       
    while (digitalRead(BTN) == LOW); // espera a soltar el botón
  }

  int nivel1 = medirNivel(sensor1);
  int nivel2 = medirNivel(sensor2);
  unsigned long ahora = millis(); // tiempo actual en milisegundos 
  
  // --- Sensor 1 ---
  if (modo == 0 && nivel1 > sens1 && (ahora - ultimaDeteccion1 > intervaloSilencio)) {
    estadoLED1 = !estadoLED1;
    digitalWrite(LED1, estadoLED1);
    ultimaDeteccion1 = ahora;
  }

  // --- Sensor 2 ---
  if (modo == 1 && nivel2 > sens2 && (ahora - ultimaDeteccion2 > intervaloSilencio)) {
    estadoLED2 = !estadoLED2;
    digitalWrite(LED2, estadoLED2);
    ultimaDeteccion2 = ahora;
  }

  // --- Actualización del LCD ---
  lcd.setCursor(0, 0);
  if (modo == 0) {
    lcd.print("Modo: S1->LED1   ");
  } else {
    lcd.print("Modo: S2->LED2   ");
  }

  lcd.setCursor(0, 1);
  if (modo == 0) {
    lcd.print("LED1: ");
    lcd.print(estadoLED1 ? "ON " : "OFF");
  } else {
    lcd.print("LED2: ");
    lcd.print(estadoLED2 ? "ON " : "OFF");
  }

  delay(100);
}

