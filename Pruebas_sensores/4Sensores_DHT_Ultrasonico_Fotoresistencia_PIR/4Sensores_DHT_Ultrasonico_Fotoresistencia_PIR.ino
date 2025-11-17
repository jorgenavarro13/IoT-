// --- Bibliotecas ---
#include <DHT.h>

// --- Definiciones Sensores (Sketch 1) ---
#define DHTTYPE DHT11
#define LDRPIN A0
const int trigPin = D3;
const int echoPin = D2;
const int LR = D5;       // Pin para el LED/Relé del ultrasónico
const int dhtPin = D1;   // Pin del sensor DHT11
float voltageLDR=0.0;
const int sensorGas=D4;

#define SOUND_SPEED 0.034

// --- Definiciones Sensores (Sketch 2) ---
const int PIRPin = D7;   // Pin 13 (GPIO 13)
const int LEDPin = D8;   // Pin 15 (GPIO 15)

// --- Intervalo del Bucle ---
// Usamos el intervalo de 200ms del Sketch 2 como el principal
const long loopInterval = 200; 

// --- Variables Globales ---
DHT dht(dhtPin, DHTTYPE);
long duration;
float distanceCm;

// =======================================================
//   CONFIGURACIÓN (SETUP)
// =======================================================
void setup() {
  // Iniciar Serial a 115200 (para PLX-DAQ)
  Serial.begin(115200);
  
  // Configuración PLX-DAQ
  Serial.println("CLEARDATA");
  // Actualizamos la etiqueta para incluir todos los datos
  Serial.println("LABEL,Estado PIR,Temperatura (C),Distancia (cm)");

  // Iniciar sensor DHT
  dht.begin();

  // Configurar pines (Sketch 1)
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(LR, OUTPUT);

  // Configurar pines (Sketch 2)
  pinMode(PIRPin, INPUT);
  pinMode(LEDPin, OUTPUT);

  //Configuramos pines de fotoresistencia

 
  
  // Estado inicial de los actuadores
  digitalWrite(LEDPin, HIGH); // Apaga el LED al inicio (Lógica Inversa)
  digitalWrite(LR, LOW);      // Apaga el relé al inicio
}

// =======================================================
//   BUCLE PRINCIPAL (LOOP)
// =======================================================
void loop() {
  // --- 1. Leer Sensor Ultrasónico (Sketch 1) ---
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_SPEED / 2;

  // --- 2. Leer Sensor DHT11 (Sketch 1) ---
  float temp = dht.readTemperature();

  // --- 3. Leer Sensor PIR (Sketch 2) ---
  int detectionValue = digitalRead(PIRPin);
  int pirState = 0; // Variable para registrar 0 o 1

  // ----3.Lectura FOTORESISTENCIA-------------
  int lecturaSensorA0=analogRead(LDRPIN);
  voltageLDR=lecturaSensorA0*(3.3/1023.0);

  //-------Lectura Fotoresistencia--------------

  //-------Lectura de GAS------------------
  float lecturaGas=analogRead(sensorGas);
  
  // --- 4. Lógica de Actuadores ---
  
  // Lógica del PIR (Sketch 2) - Corregida a Lógica Inversa
  if (detectionValue == HIGH) {
    digitalWrite(LEDPin, LOW); // LED se ENCIENDE
    pirState = 1;
  } else {
    digitalWrite(LEDPin, HIGH); // LED se APAGA
    pirState = 0;
  }

  // Lógica del Ultrasónico (Sketch 1)
  if (distanceCm < 10) {
    digitalWrite(LR, HIGH); // Activar Relé/LED
  } else {
    digitalWrite(LR, LOW);  // Desactivar Relé/LED
  }

  // --- 5. Enviar Datos por Serial (Formato PLX-DAQ) ---
  
  if (isnan(temp)) {
    Serial.println("Error DHT,,,,"); // Enviar error si el DHT falla
  } else {
    // Imprimir todos los datos en una sola línea, separados por comas
    Serial.print(pirState);//Sensor PIR (Movimiento)
    Serial.print(",");
    Serial.print(temp);//Sensor de temperatura
    Serial.print(",");
    Serial.print(distanceCm);//Ultrasonico (Distancia)
    Serial.print(",");
    Serial.println(lecturaSensorA0);//Fotoresistencia
    
    //Serial.print(",");
    //Serial.println(lecturaGas);Todavia no esta implementado porque igual es analogica y se necesita otra placa
    
  }

  // --- 6. Demora del Bucle ---
  // Pausa de 200ms antes de la siguiente lectura
  delay(loopInterval);
}
