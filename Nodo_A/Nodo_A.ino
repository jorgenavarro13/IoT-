#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Servo.h>

// --- Configuración de Pines ---
const int pinTrig = D2;      // PIN NUEVO: Trigger del Ultrasonico
const int pinEcho = D0;      // PIN NUEVO: Echo del Ultrasonico
const int pinServo = D1;     // Servo Motor
const int pinRele = D5;      // Relé del ventilador
const int pinGasLED = D6;    // LED de alerta de gas
const int pinGasAnalogico = A0; // Sensor de Gas (MQ)

// --- Variables Globales ---
Servo miServo;
long duration;
int distance;

// --- ESP-NOW --- Dirección MAC del Receptor
// REEMPLAZA ESTO CON LA MAC DE TU RECEPTOR
uint8_t broadcastAddress[] = {0x08, 0x3A, 0x8D, 0xD3, 0x69, 0xCF};

// --- Estructura de datos (Debe ser igual en el receptor) ---
typedef struct struct_message {
  bool gasPeligroso;  // true = Gas detectado
  int valorGas;       // Valor analógico
  bool tapaAbierta;   // true = Objeto detectado (tapa abierta)
} struct_message;

struct_message datosEnviar;

// Callback de envío
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  // Opcional: Debug
}

void setup() {
  Serial.begin(115200);
  
  // --- Configuración Pines ---
  pinMode(pinTrig, OUTPUT);
  pinMode(pinEcho, INPUT);
  pinMode(pinRele, OUTPUT);
  pinMode(pinGasLED, OUTPUT);
  
  // Estado inicial
  digitalWrite(pinRele, HIGH); // Apagado (Lógica inversa)
  digitalWrite(pinGasLED, LOW);

  // --- Servo ---
  miServo.attach(pinServo);
  miServo.write(0); // Cerrar al inicio

  // --- WiFi & ESP-NOW ---
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != 0) {
    Serial.println("Error inicializando ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  Serial.println("Sistema Iniciado con Ultrasonico...");
  delay(1000);
}

void loop() {
  // 1. --- LEER SENSOR ULTRASONICO ---
  // Limpiar trigger
  digitalWrite(pinTrig, LOW);
  delayMicroseconds(2);
  // Pulso de 10us
  digitalWrite(pinTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrig, LOW);
  
  // Leer Eco
  duration = pulseIn(pinEcho, HIGH);
  // Calcular distancia (cm)
  distance = duration * 0.034 / 2;

  // 2. --- LEER SENSOR GAS ---
  int valorGas = analogRead(pinGasAnalogico);

  // 3. --- LOGICA DE DATOS ---
  // Actualizar estructura
  datosEnviar.valorGas = valorGas;
  datosEnviar.gasPeligroso = (valorGas >= 650); // Umbral de gas
  
  // Si la distancia es 10cm o menos (y válida > 0), marcamos tapa abierta
  if (distance > 0 && distance <= 10) {
    datosEnviar.tapaAbierta = true;
  } else {
    datosEnviar.tapaAbierta = false;
  }

  // Debug en Serial
  Serial.print("Dist: "); Serial.print(distance); Serial.print("cm | Gas: "); Serial.println(valorGas);

  // 4. --- ENVIAR DATOS ESP-NOW ---
  // Enviamos ANTES de mover el servo para que el receptor sepa que se va a abrir
  esp_now_send(broadcastAddress, (uint8_t *) &datosEnviar, sizeof(datosEnviar));

  // 5. --- ACCIONES FISICAS LOCALES ---
  
  // A) Control de GAS (Ventilador)
  if (datosEnviar.gasPeligroso) {
    digitalWrite(pinRele, LOW); // Encender ventilador (logica inversa)
    digitalWrite(pinGasLED, HIGH);
  } else {
    digitalWrite(pinRele, HIGH);
    digitalWrite(pinGasLED, LOW);
  }

  // B) Control de SERVO (TAPA)
  // Aquí entra la lógica de bloqueo que pediste
  if (datosEnviar.tapaAbierta) {
    Serial.println("Abriendo Tapa (Ciclo)...");
    
    // --- FASE 1: ABRIR (Suave con FOR) ---
    for (int pos = 0; pos <= 180; pos += 2) { // Pasos de 2 grados
      miServo.write(pos);
      delay(15); // Controla la velocidad de apertura
    }
    
    // --- FASE 2: ESPERAR ABIERTO ---
    delay(3000); // Mantiene abierto 3 segundos
    
    // --- FASE 3: CERRAR (Suave con FOR) ---
    Serial.println("Cerrando Tapa...");
    for (int pos = 180; pos >= 0; pos -= 2) {
      miServo.write(pos);
      delay(15); // Controla la velocidad de cierre
    }

    // Al terminar el ciclo, reseteamos la variable en local para la siguiente vuelta
    // (aunque se sobrescribirá al inicio del loop con la nueva lectura)
    Serial.println("Ciclo terminado. Reanudando sensores.");
  }

  delay(200); // Pequeña pausa para estabilidad general
}
