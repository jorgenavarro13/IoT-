#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h> // No se usa aquí, pero se deja por si acaso
#include <WiFiClient.h>        // No se usa aquí, pero se deja por si acaso
#include <Servo.h>
#include <espnow.h> // --- ESP-NOW --- Librería necesaria

//-----Declaracion PINES----------
const int pinPIR = D0;
const int pinServo = D1;
const int pinGasDigital = D2;
const int pinRele = D5;       // Pin para el Relé del ventilador
const int pinGasAnalogico = A0;
const int pinGasLED = D6;

// Variable de estado Servo
Servo servo;

// --- ESP-NOW --- Dirección MAC del Receptor (Nodo B)
// Formato debe ser 0xXX, 0xXX...
//uint8_t broadcastAddress[] = {0x24, 0xD7, 0xEB, 0xCC, 0xCD, 0x6A};
uint8_t broadcastAddress[] = {0x08, 0x3A, 0x8D, 0xD3, 0x69, 0xCF};

// --- ESP-NOW --- Estructura de datos para enviar
// IMPORTANTE: Esta estructura DEBE SER IGUAL en el código del RECEPTOR
typedef struct struct_message {
  bool gasPeligroso;  // Avisa si hay peligro
  int valorGas;       // Envía el dato crudo para registro
  bool tapaAbierta;   // Avisa si la tapa se abrió
} struct_message;

// Crear una variable llamada "datosEnviar" con esa estructura
struct_message datosEnviar;

// --- ESP-NOW --- Función de callback (opcional) para saber si se envió bien
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  // Puedes descomentar esto para depurar si los mensajes salen
  // Serial.print("Estado del último envío: ");
  // if (sendStatus == 0){
  //   Serial.println("Entregado correctamente");
  // }
  // else{
  //   Serial.println("Fallo en la entrega");
  // }
}

void setup() {
  Serial.begin(115200);
  Serial.println();

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
  servo.write(0); // Asegurar tapa cerrada al inicio

  // --- ESP-NOW --- INICIALIZACIÓN
  // 1. Poner WiFi en modo Station (necesario para ESP-NOW)
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); // Nos aseguramos de no estar conectados a ningún router

  // 2. Iniciar ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error al inicializar ESP-NOW");
    return;
  }

  // 3. Configurar rol y registrar la función de callback
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER); // Este nodo es un controlador (emisor)
  esp_now_register_send_cb(OnDataSent);

  // 4. Registrar al par (el receptor)
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  
  Serial.println("Sistema Emisor iniciado con ESP-NOW...");
  delay(1000); 
}

void loop() {
  
  // 1. ------------ LEER SENSORES -------------------------
  int valorGasAnalogico = analogRead(pinGasAnalogico);
  // int valorGasDigital = digitalRead(pinGasDigital); // No lo usas en tu lógica actual, lo comento
  int lecturaPIR = digitalRead(pinPIR);

  Serial.print("Gas (ADC): ");
  Serial.print(valorGasAnalogico);
  Serial.print(" | PIR: ");
  Serial.println(lecturaPIR);

  // 2. --- ESP-NOW --- PREPARAR DATOS PARA ENVIAR
  // Llenamos la estructura con los valores leídos
  datosEnviar.valorGas = valorGasAnalogico;

  // Determinamos estado de gas peligroso para el envío
  if (valorGasAnalogico >= 650) {
      datosEnviar.gasPeligroso = true;
  } else {
      datosEnviar.gasPeligroso = false;
  }

  // Determinamos estado de la tapa para el envío
  if (lecturaPIR == HIGH) {
      datosEnviar.tapaAbierta = true;
  } else {
      datosEnviar.tapaAbierta = false;
  }

  // 3. --- ESP-NOW --- ENVIAR LOS DATOS AHORA
  // Se envían antes de las acciones físicas que pueden tener 'delays'
  esp_now_send(broadcastAddress, (uint8_t *) &datosEnviar, sizeof(datosEnviar));


  // 4. ------------ ACCIONES FÍSICAS LOCALES ----------------

  // Lógica de seguridad: Si detecta gas, enciende el ventilador
  if (datosEnviar.gasPeligroso == true) {
      Serial.println("¡GAS DETECTADO! Activando ventilador...");
      digitalWrite(pinRele, LOW); // Enciende relé (Lógica inversa)
      digitalWrite(pinGasLED, HIGH);
  } else {
      digitalWrite(pinRele, HIGH); // Apaga relé
      digitalWrite(pinGasLED, LOW);
  }

  //------------SENSOR PIR (MOVIMIENTO)------------
  if (datosEnviar.tapaAbierta == true) {
    Serial.println("ALERTA: ¡Movimiento detectado! Abriendo tapa...");
    servo.write(180);
    // NOTA: Este delay bloquea el código por 2 segundos. 
    // Durante este tiempo NO se detecta gas ni se envían datos.
    // Para un prototipo está bien, para producto final se usaría 'millis()'.
    delay(2000); 
    servo.write(0);
    Serial.println("Cerrando tapa.");
  }

  //--------------DELAY DEL LOOP--------------------
  // Reduje el delay final para que la comunicación sea más fluida.
  // 3 segundos era mucho tiempo sin actualizar datos. 
  delay(500); 
}
