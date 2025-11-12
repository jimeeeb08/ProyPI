#include <LiquidCrystal.h>

// Sensibilidad (ajustar si es necesario, valores bajos = más sensible)
const int sens1 = 8;   
const int sens2 = 8;   
const int lecturas = 100; // Se reducen cantidad de lecturas para que el programa responda más rápido

LiquidCrystal lcd(13, 12, 5, 4, 3, 2); // pines del LCD

const int sensor1 = A0;
const int sensor2 = A1;
const int LED1 = 7;
const int LED2 = 6;
const int BTN  = 8;

int modo = 0; // 0 = LED1 controlado por sensor1, 1 = LED2 controlado por sensor2

// Variables para guardar el estado actual de los LEDs (encendido/apagado)
// En verisones anteriores el LED solo dependia de la lectura en "vivo" del sensor, ahora el estado cambia cada vez que percibe un sonido haciendo que se mantenga encendido hasta que perciba otro sonido 
bool estadoLED1 = false;
bool estadoLED2 = false;

// Variable para impedir múltiples lecturas del mismo aplauso , solo se acepta nueva deteccion si pasaron al menos 700 ms del ultimo aplauso
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

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(BTN, INPUT_PULLUP);

  lcd.begin(16, 2);
}

void loop() {

  // --- Medir el tiempo total del loop en milisegundos ---
  unsigned long loopInicio = millis();

  // --- Cambio de modo con el botón ---
  if (digitalRead(BTN) == LOW) {
    modo = 1 - modo;  // alterna entre LED1 y LED2
    delay(200);       
    while (digitalRead(BTN) == LOW); // espera a soltar el botón
  }

  //--- Medir el tiempo de ejecución de medirNivel(sensor1) ---
  unsigned long t1_inicio = micros();
  int nivel1 = medirNivel(sensor1);
  unsigned long t1_fin = micros();
  unsigned long tiempoNivel1 = t1_fin - t1_inicio; // en microsegundos

  //--- Medir el tiempo de ejecución de medirNivel(sensor2) ---
  unsigned long t2_inicio = micros();
  int nivel2 = medirNivel(sensor2);
  unsigned long t2_fin = micros();
  unsigned long tiempoNivel2 = t2_fin - t2_inicio; // en microsegundos

  unsigned long ahora = millis(); // Obtiene tiempo actual en milisegundos 
  
  // --- Detección de "evento" de aplauso/soplido ---
  // Si el nivel supera el umbral, se interpreta como un "evento sonoro"
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
  // Muestra solo el LED que esta en uso
  lcd.setCursor(0, 0);  // Fila 0
  if (modo == 0) {
    lcd.print("Modo: S1->LED1   ");
  } else {
    lcd.print("Modo: S2->LED2   ");
  }

  lcd.setCursor(0, 1);  // Fila 1
  if (modo == 0) {
    lcd.print("LED1: ");
    lcd.print(estadoLED1 ? "ON " : "OFF");
  } else {
    lcd.print("LED2: ");
    lcd.print(estadoLED2 ? "ON " : "OFF");
  }

  // --- Medir tiempo total del loop (en milisegundos) ---
  unsigned long loopFin = millis();
  unsigned long tiempoLoop = loopFin - loopInicio;

  // --- Mostrar resultados en el Monitor Serial ---
  Serial.print("Tiempo medirNivel(S1): ");
  Serial.print(tiempoNivel1);
  Serial.print(" us | medirNivel(S2): ");
  Serial.print(tiempoNivel2);
  Serial.print(" us | Tiempo total loop: ");
  Serial.print(tiempoLoop);
  Serial.println(" ms");
  
  delay(100); // pequeña pausa de estabilidad
}
