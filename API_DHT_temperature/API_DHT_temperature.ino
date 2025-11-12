#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#define DHTTYPE DHT11
//int D1 = 5; // Pin D1 en NodeMCU (corresponde a GPIO5)
DHT dht(D1, DHTTYPE);



// --- CONFIGURA ESTO ---
//ssid, password y " .ttp://xxxxx:3000/"

const char* ssid = "Carlos's sister GOD";
const char* password = "12345678";

// 1. Reemplaza por la IP local de tu PC (ej. 192.168.1.100)
// 2. El endpoint es /insertTemperature, como en constants.js
String serverName = "http://10.235.177.244:3000/iot/api/insertTemperature";
// ---------------------

void setup() {
  Serial.begin(9600);
  dht.begin();

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
  // Espera 10 segundos entre envíos
  delay(10000);

  float temp = dht.readTemperature();

  if (isnan(temp)) {
    Serial.println("Error leyendo el sensor DHT11");
    return;
  }

  Serial.print("Temperatura leída: ");
  Serial.println(temp);

  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/json");

    // Crear el JSON. Debe coincidir con lo que espera la API: {"valor": "XX.XX"}
    //
    String httpRequestData = "{\"valor\":\"" + String(temp) + "\"}";

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
}
