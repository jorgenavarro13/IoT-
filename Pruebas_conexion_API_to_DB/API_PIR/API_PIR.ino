#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Servo.h>

//-----Declaracion PINES----------
const int pinPIR = D0;
const int pinServo = D1;
const int pinGasDigital = D2;
const int pinRele = D5;      // NUEVO: Pin para el Relé del ventilador
const int pinGasAnalogico = A0;
const int pinGasLED = D6;

// Variable de estado
Servo servo;

void setup() {
  Serial.begin(115200);

  //---------CONFIGURACION PINES-------
  pinMode(pinPIR, INPUT);
  pinMode(pinGasDigital, INPUT);
  
  // Configurar el Relé como salida
  pinMode(pinRele, OUTPUT);
  pinMode(pinGasLED, OUTPUT);
  digitalWrite(pinRele, HIGH); // Inicialmente apagado (para relés de lógica inversa)
  digitalWrite(pinGasLED, LOW);

  //---------SERVO-------
  servo.attach(pinServo);
  
  Serial.println("Sistema iniciado...");
  delay(1000); 
}

void loop() {
  
  //------------SENSOR GAS-------------------------
  int valorGasAnalogico = analogRead(pinGasAnalogico);
  int valorGasDigital = digitalRead(pinGasDigital);
  
  Serial.print("Gas (ADC): ");
  Serial.print(valorGasAnalogico);
  Serial.print(" | Gas (Digital): ");
  Serial.println(valorGasDigital);

  // Lógica de seguridad: Si detecta gas, enciende el ventilador
  // (Ojo: Los sensores digitales de gas suelen dar 0 cuando detectan y 1 cuando no, 
  // o viceversa. Ajusta según tu sensor).
  if (valorGasAnalogico>=650) { // Asumiendo LOW = Gas detectado
      Serial.println("¡GAS DETECTADO! Activando ventilador...");
      digitalWrite(pinRele, LOW); // Enciende relé (Lógica inversa)
      digitalWrite(pinGasLED, HIGH);
  } else {
      digitalWrite(pinRele, HIGH); // Apaga relé
      digitalWrite(pinGasLED, LOW);
  }

  //------------SENSOR PIR (MOVIMIENTO)------------
  int lecturaPIR = digitalRead(pinPIR);
  
  if (lecturaPIR == HIGH) {
    Serial.println("ALERTA: ¡Movimiento detectado!");
    servo.write(180);
    delay(2000);
    servo.write(0);
  } else {
    Serial.println("Estado: Zona tranquila");
  }

  //--------------SERVO Y PRUEBA RELE--------------------
  delay(3000);
}
