// Librerias
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
// --- ESP-NOW --- Librerías Adicionales
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h> // <--- CAMBIO 1: Importante para HTTPS

//------------------Conexión internet---------------
const char* ssid = "Tec-IoT";
const char* password = "spotless.magnetic.bridge";

//http://192.168.1.40:3000/iot/api/insertAllSensors
String serverName = "https://api-iot-6t7d.onrender.com/iot/api/insertAllSensors";

// ================= DEFINICIONES ESP-NOW =================
// --- IMPORTANTE: Esta estructura DEBE SER EXACTAMENTE IGUAL a la del EMISOR ---
typedef struct struct_message {
  bool gasPeligroso;  // Si es true, hay que sonar alarma
  int valorGas;       // Valor crudo (opcional mostrarlo)
  bool tapaAbierta;   // Si es true, la tapa se está abriendo
} struct_message;

struct_message datosRecibidos; // Variable donde guardaremos los datos que llegan
bool datosFrescos = false;     // Bandera para saber si acaba de llegar info

// Función "Callback": Se ejecuta automáticamente cuando llega un mensaje
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&datosRecibidos, incomingData, sizeof(datosRecibidos));
  datosFrescos = true; // Indicamos que llegó data nueva
}
// ========================================================


// Declaracion de la pantalla
const int ancho = 128;
const int alto  = 32;
const int scl = 13; // D7
const int sda = 2;  // D4

// Pines para buzzer y fotoresistencia
const int buzzer = 15; // D8
const int FotoresistenciaPin = A0; // Renombrado para claridad
const int LEDPIN = 14; // D5 (Luz de cortesía)
int umbralLuz = 400;   // Umbral para detectar oscuridad

// Pines sensor ultrasonico
const int trigPin = 0; // D3
const int echoPin = 4; // D2
const int LR = 12;   // D6 (LED indicador de lleno local)

// Definicion pines DHT11
const int dhtPin = 5; // D1
DHT dht(dhtPin, DHT11);  // ✅ objeto DHT

// Constante velocidad sonido
#define SOUND_SPEED 0.034  // cm/us

// Intervalo del bucle (Reducido para mejor respuesta de la pantalla)
const long loopInterval = 1000; // Un poco más largo para no saturar la API

// Objeto pantalla
Adafruit_SSD1306 display(ancho, alto, &Wire, -1);

void setup() {
  Serial.begin(115200);
  Serial.println("\nIniciando Nodo Receptor...");

  // Serial Data Streamer header
  Serial.println("CLEARDATA");
  Serial.println("LABEL,Temperatura (C), Distancia (cm), Luz, GasDetectado(Bool)");

  // Iniciar sensor DHT
  dht.begin();

  // Configurar pines
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(LR, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(LEDPIN, OUTPUT);

  // Inicialización actuadores APAGADOS
  digitalWrite(LR, LOW);
  digitalWrite(trigPin, LOW);
  digitalWrite(LEDPIN, LOW);
  digitalWrite(buzzer, LOW);

  // Pantalla
  Wire.begin(sda, scl);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("No se encuentra la pantalla OLED"));
    for(;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Iniciando ESP-NOW...");
  display.display();
  delay(1000);

  // ================= INICIO ESP-NOW =================
  WiFi.mode(WIFI_STA); // Modo estación necesario
  WiFi.disconnect();   // Aseguramos que no intente conectarse a router por ahora

  if (esp_now_init() != 0) {
    Serial.println("Error al inicializar ESP-NOW");
    display.clearDisplay(); display.setCursor(0,0); display.print("Error ESP-NOW"); display.display();
    return;
  }
  // Configurar rol de esclavo (receptor) y registrar la función que recibe
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("ESP-NOW Receptor Listo. Esperando datos...");
  // ==================================================

  // Inicializar datos recibidos en falso por seguridad
  datosRecibidos.gasPeligroso = false;
  datosRecibidos.tapaAbierta = false;
  datosRecibidos.valorGas = 0;


  WiFi.begin(ssid, password);
  Serial.println("Conectando a WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado a WiFi");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Sistema Listo");
  display.display();
  delay(500);
}

void loop() {
  // --- 1. Leer Sensores Locales ---
  // Ultrasónico
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  int duration = pulseIn(echoPin, HIGH);
  int distanceCm = duration * SOUND_SPEED / 2;
  if (distanceCm == 0) distanceCm = 999; // Evitar ceros erróneos

  // DHT11
  float temp = dht.readTemperature();
  if (isnan(temp)) temp = 0.0; // Manejo básico de error

  // Fotoresistencia (LDR)
  int lecturaLuz = analogRead(FotoresistenciaPin);


  // --- 2. Lógica de Actuadores Locales (No críticos) ---
  
  // LDR controla SOLAMENTE el LED de cortesía (LEDPIN)
  if (lecturaLuz < umbralLuz) {
    digitalWrite(LEDPIN, HIGH); // Está oscuro, encender luz
  } else {
    digitalWrite(LEDPIN, LOW);  // Hay luz, apagar led
  }

  // LED indicador local de "casi lleno" (LR)
  if (distanceCm < 15 && distanceCm > 10) {
    digitalWrite(LR, HIGH);
  } else {
    digitalWrite(LR, LOW);
  }


  // --- 3. LÓGICA PRINCIPAL DE PANTALLA Y ALERTAS (PRIORIDADES) ---
  
  display.clearDisplay();
  display.setTextColor(WHITE);

  // --- PRIORIDAD ALTA: ALERTA DE GAS (Viene por ESP-NOW) ---
  if (datosRecibidos.gasPeligroso == true) {
    digitalWrite(buzzer, HIGH);   // ¡Sonar alarma!
    
    display.setTextSize(2);       // Texto grande
    display.setCursor(10, 0);
    display.println("!PELIGRO!");
    display.setTextSize(1);       // Texto normal
    display.setCursor(25, 20);
    display.println("GAS DETECTADO");

  } 
  // --- PRIORIDAD MEDIA: BOTE LLENO (Sensor local) ---
  else if (distanceCm < 10) {
    digitalWrite(buzzer, LOW);  // Apagar buzzer si estaba prendido por gas antes
    
    display.setTextSize(2);
    display.setCursor(30, 0);
    display.println("LLENO");
    display.setTextSize(1);
    display.setCursor(20, 20);
    display.print("Nivel: "); display.print(distanceCm); display.print("cm");
  }
  // --- PRIORIDAD BAJA: ESTADO NORMAL ---
  else {
    digitalWrite(buzzer, LOW); // Todo tranquilo
    
    display.setTextSize(1);
    
    // Línea 1: Saludo basado en luz (como tenías antes)
    display.setCursor(0, 0);
    if (lecturaLuz < umbralLuz) display.print("Buenas noches. ");
    else display.print("Buenos dias. ");
    
    // Línea 2: Datos de sensores
    display.setCursor(0, 12);
    display.print("T:"); display.print((int)temp); display.print("C ");
    display.print("Dist:"); display.print(distanceCm); display.print("cm");
    
    // Línea 3: Aviso de tapa (Viene por ESP-NOW)
    display.setCursor(0, 24);
    if (datosRecibidos.tapaAbierta) {
       display.print(">> TAPA ABIERTA <<");
    } 
    else if (!datosRecibidos.tapaAbierta) {
       display.print(">> TAPA CERRADA <<");
    }
    else {
       // Opcional: Mostrar valor crudo del gas si no es peligroso
       display.print("Gas Level: "); display.print(datosRecibidos.valorGas); display.print(">> TAPA CERRADA <<");
    }
  }
  
  display.display(); // Actualizar la pantalla al final de la lógica


  // --- 4. Enviar Datos por Serial (Excel Data Streamer) ---
  // Usamos "DATA,..." para que Excel lo reconozca
  Serial.print("DATA,");
  Serial.print(temp);
  Serial.print(",");
  Serial.print(distanceCm);
  Serial.print(",");
  Serial.print(lecturaLuz);
  Serial.print(",");
  Serial.println(datosRecibidos.gasPeligroso); // Agregamos el estado del gas al serial

  //-----------Enviamos datos a DB usando la API-----------------
  int valorTapaAbierta = datosRecibidos.tapaAbierta ? 1 : 0;
  
  if (WiFi.status() == WL_CONNECTED) {
    
    // <--- CAMBIO 2: Usar WiFiClientSecure en lugar de WiFiClient
    WiFiClientSecure client;
    
    // <--- CAMBIO 3: Ignorar validación de certificado SSL (Necesario para Cloudflare/Render simple)
    client.setInsecure(); 

    HTTPClient http;

    // Pasamos el cliente seguro a http.begin
    if (http.begin(client, serverName)) { 
    
        http.addHeader("Content-Type", "application/json");

        // Crear el JSON.
        //level, temperature, light, gas, open
        String httpRequestData = 
          "{"
            "\"level\":\"" + String(distanceCm) + "\"," 
            "\"temperature\":\"" + String(temp) + "\"," 
            "\"light\":\"" + String(lecturaLuz) + "\"," 
            "\"gas\":\"" + String(datosRecibidos.valorGas) + "\"," 
            "\"open\":\"" + String(valorTapaAbierta) + "\""
          "}";

        
        Serial.print("Enviando JSON Seguro: ");
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
        Serial.println("No se pudo conectar al servidor HTTPS");
    }
  } else {
    Serial.println("Error: WiFi desconectado");
  }


  // --- 5. Delay y Reset de bandera ---
  datosFrescos = false; // Reseteamos la bandera (opcional, para lógica futura)
  delay(loopInterval);
}
