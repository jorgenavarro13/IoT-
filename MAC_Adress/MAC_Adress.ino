#include <ESP8266WiFi.h>
void setup(){
   Serial.begin(115200);
   Serial.println();
   Serial.print("Mi Dirección MAC es: ");
   Serial.println(WiFi.macAddress());
}
void loop(){
  Serial.println();
   Serial.print("Mi Dirección MAC es: ");
   Serial.println(WiFi.macAddress());
   
   
   }
