// Definición de pines para el LED RGB
// ¡IMPORTANTE! D0, D1, D2 en el NodeMCU (ESP8266) corresponden a GPIO16, GPIO5, y GPIO4, respectivamente.
// Este código usa las constantes D0, D1, D2 que son reconocidas por el IDE.
const int PIN_RED = D0; 
const int PIN_GREEN = D1;
const int PIN_BLUE = D2;

// Tiempo que cada color estará encendido (en milisegundos)
const int DELAY_TIME = 1000; 

// Estado para encender (HIGH) o apagar (LOW) para un LED de CÁTODO COMÚN.
// Encender = HIGH (proveer 3V)
// Apagar = LOW (proveer GND)
const int ON = HIGH;
const int OFF = LOW;

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando prueba de LED RGB (Catodo Comun)...");
  
  // Configurar los pines como salidas
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  
  // Asegurarse de que el LED esté apagado al inicio
  turnOffLEDs();
}

void loop() {
  Serial.println("Encendiendo ROJO...");
  digitalWrite(PIN_RED, ON);
  delay(DELAY_TIME);
  turnOffLEDs(); // Apagar el color actual
  delay(500); // Pausa breve entre colores

  Serial.println("Encendiendo VERDE...");
  digitalWrite(PIN_GREEN, ON);
  delay(DELAY_TIME);
  turnOffLEDs();
  delay(500);

  Serial.println("Encendiendo AZUL...");
  digitalWrite(PIN_BLUE, ON);
  delay(DELAY_TIME);
  turnOffLEDs();
  delay(500);

  Serial.println("Secuencia completada. Repitiendo en 2 segundos.");
  delay(2000); // Pausa más larga antes de repetir la secuencia
}

/**
 * Función auxiliar para apagar todos los colores del LED.
 */
void turnOffLEDs() {
  digitalWrite(PIN_RED, OFF);
  digitalWrite(PIN_GREEN, OFF);
  digitalWrite(PIN_BLUE, OFF);
}
