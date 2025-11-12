// --- Definición de Pines ---
const int PIN_POTENCIOMETRO = A0; 
const int PIN_RED = D0;     // GPIO16
const int PIN_GREEN = D1;   // GPIO5
const int PIN_BLUE = D2;    // GPIO4

// --- Variables ---
int valorPotenciometro = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("NodeMCU: Potenciometro controla brillo del LED ROJO...");
  
  // Configurar los pines digitales como salidas para PWM
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  
  // Apagar todos los colores al inicio
  turnOffLEDs();
}

void loop() {
  // 1. Lectura del Sensor (Potenciómetro)
  // El valor leído va de 0 a 1023
  valorPotenciometro = analogRead(PIN_POTENCIOMETRO); 

  // 2. Control del Actuador (LED RGB - Cátodo Común)
  
  // Para un LED de cátodo común, para hacerlo más brillante, 
  // necesitamos que el valor de PWM (duty cycle) sea ALTO.
  // Como la lectura del potenciómetro ya está en el rango 0-1023 del PWM del ESP8266, 
  // podemos usar el valor directamente para el brillo.
  
  // Nota: La función analogWrite() en el ESP8266 acepta valores de 0 (apagado) a 1023 (brillo máximo).
  
  // Controlar el brillo del ROJO
  analogWrite(PIN_RED, valorPotenciometro); 
  
  // Mantener los otros colores apagados (brillo 0)
  analogWrite(PIN_GREEN, 0); 
  analogWrite(PIN_BLUE, 0); 
  
  // 3. Monitoreo (Opcional)
  Serial.print("Valor del Potenciometro (0-1023): ");
  Serial.print(valorPotenciometro);
  Serial.print(" -> Brillo ROJO (0-1023): ");
  Serial.println(valorPotenciometro);

  // Pequeña pausa
  delay(10); 
}

/**
 * Función auxiliar para asegurar que todos los colores del LED estén apagados inicialmente.
 */
void turnOffLEDs() {
  // Usar analogWrite(0) para apagar los pines completamente
  analogWrite(PIN_RED, 0);
  analogWrite(PIN_GREEN, 0);
  analogWrite(PIN_BLUE, 0);
}
