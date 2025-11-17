#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

//Antes problemas ten seleccionado la version 0.9 de NodeMCU
//---Definicion pin FOTORESISTENCIA---
#define LDR A0



// --- CONFIGURA ESTO ---
//ssid, password y " .ttp://xxxxx:3000/"

const char* ssid = "Redmi 9T";
const char* password = "cacacaca";

// 1. Reemplaza por la IP local de tu PC (ej. 192.168.1.100)
// 2. El endpoint es /insertTemperature, como en constants.js

String serverName = "http://10.215.88.244:3000/iot/api/insertFotoresistencia";


void setup() {
  // Configurar los pines
  
  // Iniciar la comunicación serial para mostrar la distancia
  Serial.begin(9600);
  Serial.println("Prueba de Sensor Fotoresistencia LDR Iniciada");

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
  float lecturaFotoresistencia=analogRead(LDR);
  
  Serial.print("Fotoresistencia ");
  Serial.print(lecturaFotoresistencia);
  Serial.println("");

  // Esperar un momento antes de la siguiente lectura

if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/json");

    // Crear el JSON. Debe coincidir con lo que espera la API: {"valor": "XX.XX"}
    //
    String httpRequestData = "{\"valor\":\"" + String(lecturaFotoresistencia) + "\"}";

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



  
  delay(2000);
}
