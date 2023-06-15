#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#define RELE 4
#define SSID "AEPA"
#define PSK "CNtec20231"
#define URL "http://192.168.1.65:8888/"

WiFiMulti wifi;
//int o float 
String jwt;
bool sesion;
void setup() {
  pinMode(RELE, OUTPUT);//pin de salida
  Serial.begin(9600);
  //conexion wifi
  wifi.addAP(SSID, PSK);
  while (wifi.run() != WL_CONNECTED) ;
  Serial.println("Conectado");
}

void loop() {
  //aqui lo que se ejecute


  if (sesion){
    //registro();
    leer();
  }
  else login();
  delay(700);
}

void login() {
  HTTPClient http;
  http.begin(URL "login");
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST("{\"username\":\"alex\", \"password\":\"123\"}");      /*aqui el login de usuario*/
  if (httpCode > 0) {
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    if (httpCode == 200) {
      String payload = http.getString();
      int p;
      p = payload.indexOf("}");
      jwt = payload.substring(10, p - 2);
      Serial.println(jwt);
      sesion = true;
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

void registro() {
  HTTPClient http;
  http.begin(URL "reg");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + jwt);
  int httpCode = http.POST("{\"name\":\"ELECTROs\",\"value\":\"0\"}");
  if (httpCode > 0) {
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    if (httpCode == 201) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

void leer() {
  HTTPClient http;
  http.begin(URL "elec/1");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + jwt);
  int httpCode = http.GET();
  if (httpCode > 0) {
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    if (httpCode == 200) {
      String payload = http.getString();
      Serial.println(payload);
      int p;
      String x;
      p = payload.indexOf("isOn");
      x = payload.substring(p+6, p+7);
      if(x=="1"){
        Serial.println("ABIERTO...");
        digitalWrite(RELE, HIGH);
      }else{
        Serial.println("Cerrado...");
        digitalWrite(RELE, LOW);
      }
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}
