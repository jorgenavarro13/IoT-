// El pin A0 es el único pin de entrada analógica disponible en la mayoría de las placas NodeMCU ESP8266.
const int PIN_POTENCIOMETRO = A0; 

// Variable para almacenar el valor leído
int valorSensor = 0;

void setup() {
  // Iniciar la comunicación serial a una velocidad común (baud rate)
  Serial.begin(115200); 
  Serial.println("Iniciando lectura del Potenciómetro en el pin A0...");
  
  // No es estrictamente necesario para entradas analógicas en ESP8266, 
  // pero es buena práctica para indicar que el pin será usado como entrada.
  pinMode(PIN_POTENCIOMETRO, INPUT); 
}

void loop() {
  // La función analogRead() lee el voltaje en el pin A0.
  // El ESP8266 tiene una resolución ADC de 10 bits, 
  // por lo que el valor de retorno estará entre 0 (0V) y 1023 (1V, aunque internamente el ADC puede ser mapeado a un voltaje más alto si se usa un divisor de voltaje).
  valorSensor = analogRead(PIN_POTENCIOMETRO); 

  // Imprimir el valor leído en el monitor serial
  Serial.print("Valor del Potenciómetro (0-1023): ");
  Serial.println(valorSensor);

  // Pequeña pausa para no saturar el monitor serial
  delay(500); 
}
