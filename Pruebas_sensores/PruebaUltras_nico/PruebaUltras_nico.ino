#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>


// --- CONFIGURA ESTO ---
//ssid, password y " .ttp://xxxxx:3000/"

const char* ssid = "AndroidAP";
const char* password = "11021102";

// 1. Reemplaza por la IP local de tu PC (ej. 192.168.1.100)
// 2. El endpoint es /insertTemperature, como en constants.js

String serverName = "http://10.38.111.244:3000/iot/api/insertUltrasonic";

// Definición de pines
const int trigPin = D3; // Pin de Emisión (Trigger)
const int echoPin = D2; // Pin de Recepción (Echo)

// Variables para el cálculo de la distancia
long duracion; // Almacena la duración del pulso (en microsegundos)
float distanciaCm; // Almacena la distancia calculada (en centímetros)

void setup() {
  // Configurar los pines
  pinMode(trigPin, OUTPUT); // El pin Trig debe ser de SALIDA
  pinMode(echoPin, INPUT);  // El pin Echo debe ser de ENTRADA

  // Iniciar la comunicación serial para mostrar la distancia
  Serial.begin(9600);
  Serial.println("Prueba de Sensor Ultrasónico Iniciada");

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
  // 1. Limpiar el pin Trig (asegurarse de que esté BAJO)
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // 2. Enviar pulso de 10µs por el pin Trig para iniciar la medición
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // 3. Leer el pin Echo: devuelve la duración del viaje de la onda de sonido
  duracion = pulseIn(echoPin, HIGH);

  // 4. Calcular la distancia
  // Distancia = (Duración del Pulso / 2) / 29.1
  // El 29.1 (o 29.4) es el tiempo (en microsegundos) que tarda el sonido en recorrer 1 cm.
  // Dividimos por 2 porque 'duracion' es el tiempo de ida Y vuelta.
  distanciaCm = duracion * 0.034 / 2;
  
  // O de forma equivalente:
  // distanciaCm = duracion / 58;

  // 5. Imprimir la distancia en el monitor serial
  Serial.print("Distancia: ");
  Serial.print(distanciaCm);
  Serial.println(" cm");

  // Esperar un momento antes de la siguiente lectura

if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/json");

    // Crear el JSON. Debe coincidir con lo que espera la API: {"valor": "XX.XX"}
    //
    String httpRequestData = "{\"distance\":\"" + String(distanciaCm) + "\"}";

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
