
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>


const char* ssid = "IZZI-2F56";
const char* password = "RtemPaFA";

// 1. Reemplaza por la IP local de tu PC (ej. 192.168.1.100)
// 2. El endpoint es /insertTemperature, como en constants.js

String serverName = "http://192.168.1.40:3000/iot/api/insertPIR";

// Definición de pines para la prueba
const int pinPrueba = D2;

// Variable de estado
int estado = 0;

void setup() {
  Serial.begin(9600);
  
  // *** CAMBIO CRUCIAL: Usamos INPUT_PULLUP ***
  // Esto activa la resistencia interna que fuerza al pin a HIGH por defecto.
  // Ahora, el pin solo será LOW cuando el sensor PIR lo active.
  pinMode(pinPrueba, INPUT_PULLUP); 

  Serial.println("--- PRUEBA DE DEBUGGING CON PULLUP INTERNO INICIADA ---");
  Serial.println("Estado por defecto: ALTO (HIGH)");
  Serial.println("Movimiento detectado: BAJO (LOW)");



    //Configuracion cliente wifi
  WiFi.begin(ssid, password);
  Serial.println("Conectando a WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado a WiFi");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  // Leer el estado del pin
  estado = digitalRead(pinPrueba);

  // Comprobación de estado y mensajes
  if (estado == LOW) {
    // Si el PULLUP está activo, LOW significa que el sensor ha tirado el voltaje a tierra.
    Serial.println(">>> ¡Movimiento / Señal Detectada! (Pin en LOW) <<<");
  } else {
    // HIGH es el estado inactivo, forzado por la resistencia PULLUP.
    Serial.println("Pin inactivo (Pin en HIGH)");
  }

  // Imprimir el valor real (0 para LOW, 1 para HIGH)
  Serial.print("Valor crudo del pin: ");
  Serial.println(estado);


if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/json");

    // Crear el JSON. Debe coincidir con lo que espera la API: {"valor": "XX.XX"}
    //
    String httpRequestData = "{\"estado\":\"" + String(estado) + "\"}";

    Serial.print("Enviando JSON: ");
    Serial.println(httpRequestData);

    // Enviar la petición POST
    int httpResponseCode = http.POST(httpRequestData);

    if (httpResponseCode > 0) {
      Serial.print("Código de respuesta HTTP: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    } else {
      Serial.print("Error en el envío POST. Código: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("Error: WiFi desconectado");
  }


  

  // Esperar 3 segundo
  delay(3000);
}
