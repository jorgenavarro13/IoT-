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



## Connection to Excel Video
```
https://www.youtube.com/watch?v=b1D_MJvfXIg
````


## Buzzer info
```
https://programarfacil.com/blog/arduino-blog/buzzer-con-arduino-zumbador/
```


## DHT info
```
https://programarfacil.com/blog/arduino-blog/sensor-dht11-temperatura-humedad-arduino/
```






