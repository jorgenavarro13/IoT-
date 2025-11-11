# IoT- Bote de basura inteligente
Development of IoT proyect (3rd) semester

Este repositorio intenta recabar información relevante para la realización del proyecto de IoT, destacando highlihts e hitos sobre la realización del mismo, la documentación y código que ayudo a la elaboración del proyecto

### Bienvenido a NodeMCU
<!--![Diagrama de NODE MCU](/nodemcu_pinout.png)-->
<img src="/nodemcu_pinout.png" alt="Pines_NodeMCU" style="width:300px;"/>

NodeMCU es **placa de desarrollo**  de hardware y software de código abierto para proyectos de Internet de las Cosas (IoT), utiliza el **chip microcontrolador ESP-8266**. Siendo una herramienta muy poderosa sabiendolo ocupar.

## Datos importantes 

- Tiene módulo de WiFi
- Los pines los tiene declarados diferentes a los establecidos en la carcasa, para evitarte problemas checa el datasheet y declara los pines desde un principio como en el siguiente ejemplo.
  ```
  int D1=5;
  ```
- Empieza realizando pruebas unitarias antes de empezar tu desarrollo, corrobora alimentación, comunicación serial, establecimiento de red y más para evitar problemas al debuggear.
- Tiene el chip ESP8266, para buscar información utiliza este chip como referencia

#### Errores comunes
- **No detecta puerto:** Al intentar cargar un programa marca error por puerto desconocido o no encontrado, este problema lo puedes solucionar de varias formas.
    - Checa la conexión correcta del cable, corrobora que este bien conectado o cambialo de puerto en la computadora
    - Corrobora que se esta detectando el puerto en la computadora
      ```
      Windows+x -> Device Manager -> COM 
      ```
      Si esta conectado el dispositivo y *no detecta ningún COM*, significa que ni siquiera la computadora lo esta detectando correcctamente.
      Si *lo detecta*, pero con un símbolo de advertencia puedes seleccionar el COM, meterte a sus ajustes e instalar los drivers necesarios para su funcionamiento o utilizar los otros 2 puntos para que funcione.
    - Reinicia la compu

- **No detecta el board:** Antes de pasar el programa a la placa, tienes que asegurarte tener los boards necesarios para su funcionamiento, para ello:
- Introduce
  ```
  http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
   en ```File ->Preferences -> Aditional Boards Manager URL's```
  
  -Dirigete a
  
  ```
  Tools->Boards->Board Manager
  ```
Busca esp8266 by Arduino e instala la última versión, esto permitirá poder establecer una conexión correcta con el NodeMCU


### Pruebas para conexión

Antes de empezar a programar debemos asegurarnos de que todas las dependecias y librerías esten listas para la ejecución, es por eso que primero vamos a ejecutar algunas pruebas de conexión y funcionamiento.

**Prueba prendiendo un LED** 
Inserta el siguiente código y haz el siguiente circuito para comprobar el correcto funcionamiento.

```
void setup() {
 pinMode(5, OUTPUT); // GPIO05, Digital Pin D1
}
void loop() {
 digitalWrite(5, HIGH);
 delay(900);
 digitalWrite(5, LOW);
 delay(500);
}
```
<img src="/images/pruebaBlinkConexion.png" alt="ConexionPruebaBlink" style="width:300px;"/>

Y el LED debería estar parpadeando, si no tienes LED y resistencia de 220 ohms a la mano, podrías probar el siguiente código con el led incluido de la placa.
```
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);  // Initialize the LED_BUILTIN pin as an output
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, LOW);  // Turn the LED on (Note that LOW is the voltage level
  // but actually the LED is on; this is because
  // it is active low on the ESP-01)
  delay(1000);                      // Wait for a second
  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  delay(2000);                      // Wait for two seconds (to demonstrate the active low LED)
}
```
Debería parpadear, igual puedes hacer algunas pruebas accediendo desde ```File ->Examples->ESP8266``` 


Para más información puedes acceder al siguiente enlace
```
https://handsontec.com/dataspecs/module/esp8266-V13.pdf
```

## Prueba con sensores
Ahora comprobadas las librerías y el funcionamiento correcto del NodeMCU, podemos empezar con los sensores. Para este y demás ejemplos se utilizará el sensor de temperatura DHT11
Primero debes de instalar las librerías necesarias para tu sensor en ```Tools -> Manage Libraries``` y buscar la adecuada.
En nuestro caso es la DHT sensor library by Adafruit, instala la última versión y ejecuta el script para la prueba de este sensor

```
Script
```

<img src="/images/ConexionesDHTPrueba.png" alt="ConexionesDHTPrueba" style="width:300px;"/>

Si todo sale bien, deberías ver el monitor serial con los mensajes de la temperatura y humedad a tiempo real.

Adicionalmente puedes consultar el siguiente sitio para obtener más información sobre el sensor de temperatura

```
https://programarfacil.com/blog/arduino-blog/sensor-dht11-temperatura-humedad-arduino/
```

Ahora con esto listo, podemos empezar la vinculación de los datos a una Base de Datos como Excel para capturarlos de otra manera.

## Conexión a EXCEL

El siguiente video detalla bastante bien como poder conectar nuestro sensor de temperatura a nuestra base de datos en Excel

```
https://www.youtube.com/watch?v=b1D_MJvfXIg
````

## Conexión a base de datos real

La parte más importante del proyecto es la captura de los datos de manera remota usando una red WiFi (IoT).
Para ello debes de dirigirte al documento, donde encontraras el repositorio a instalar junto con algunas indicaciones.
```
/API IoT_Reto
```

A continuación detallaremos más en algunas cuestiones
-Antes de empezar corrobora tener instaladas las dependencias con: 
```
node -v
npm -v
```

- Si te marca error al ejecutar el script desde la terminal, simplemente ingresa al backup de la base de datos y ejecuta el script directamente línea a línea, desde la terminal para abrir mysql sería.
  ```
  mysql -u USUARIO(normalmente root) -p  -P TU_PUERTO

  ```
  Te va pedir tu contraseña y podrás ejecutar los comandos de mySQL de manera directa.
  O puedes buscar cualquier administrador de tu preferencia MAMP, DBeaver, etc. Para saber usario y contraseña en MAMP, simplemente ve al sitio web después de habilitar MAMP y listo.


Para saber tu dirección IP ejecuta el siguiente comando 
```
ipconfig
```
Para saber tu conexión a internet y la contraseña utiliza los siguientes
```
netsh wlan show interfaces
netsh wlan show profile name="RedWiFi_a_la que estas conectado" key=clear
```
La contraseña aparecerá en Key Content



## Script para conexión 
Deberás modificar los valores de red WiFi, contraseña y dirección IP dependiendo de tu dispositivo y la red que utilices. Para hacer pruebas se tiene el siguiente script con el sensor DHT

```
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#define DHTTYPE DHT11
int D1 = 5; // Pin D1 en NodeMCU (corresponde a GPIO5)
DHT dht(D1, DHTTYPE);

// --- CONFIGURA ESTO ---
const char* ssid = "IZZI-2F56";
const char* password = "RtemPaFA";

// 1. Reemplaza por la IP local de tu PC (ej. 192.168.1.100)
// 2. El endpoint es /insertTemperature, como en constants.js
String serverName = "http://192.168.1.26:3000/iot/api/insertTemperature";
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
```


## Conectar con base de datos con la terminal 
```
mysql -u root -p -P 3307
show databases
use iot
```


## Buzzer info
```
https://programarfacil.com/blog/arduino-blog/buzzer-con-arduino-zumbador/
```









