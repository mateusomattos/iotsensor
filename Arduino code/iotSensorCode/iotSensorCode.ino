#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <Ticker.h>
#include "DHT.h"
#include <NTPClient.h>//Biblioteca do NTP.
#include <WiFiUdp.h>//Biblioteca do UDP.


// Set these to run example.
#define FIREBASE_HOST "iotsensor-cc9bd.firebaseio.com"
#define FIREBASE_AUTH "AXr5h8eoQmofPVUyM6pOZTBgNn30oAmTBKuZUVaP"

//#define WIFI_SSID "Vodafone-EBCF85"
//#define WIFI_PASSWORD "11ACAAE8F8"
#define WIFI_SSID "LGG6"
#define WIFI_PASSWORD "12345678"

#define LAMP_PIN D3
#define PRESENCE_PIN D4
#define DHT_PIN D5
#define DHTTYPE DHT11
// Publique a cada 5 min
#define PUBLISH_INTERVAL 1000*10*1

DHT dht(DHT_PIN, DHTTYPE);
Ticker ticker;
bool publishNewState = true;

void publish(){
  publishNewState = true;
}

void setupPins(){

  pinMode(LAMP_PIN, OUTPUT);
  digitalWrite(LAMP_PIN, LOW);
  
  //pinMode(PRESENCE_PIN, INPUT);

  dht.begin();  
}

void setupWifi(){
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
}

void setupFirebase(){
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  //Firebase.setBool("lamp", false);
  //Firebase.setBool("presence", false);
}


WiFiUDP udp;//Cria um objeto "UDP".
NTPClient ntp(udp, "a.st1.ntp.br", 0* 3600, 60000);//Cria um objeto "NTP" com as configurações.



void setup() {
  Serial.begin(115200);

  setupPins();
  setupWifi();    

  setupFirebase();

  // Registra o ticker para publicar de tempos em tempos
  ticker.attach_ms(PUBLISH_INTERVAL, publish);
  ntp.begin();//Inicia o NTP.
  ntp.forceUpdate();//Força o Update.
}

void loop() {

  float humidity = 0;
  bool fail = true;
  
  // Apenas publique quando passar o tempo determinado
  if(publishNewState){
    Serial.println("Publish new State");
    // Obtem os dados do sensor DHT 
    // Humidade
    humidity = dht.readHumidity();
    Serial.print("Humidity:");
    Serial.println(humidity);

    // Temperatura
    float temperature = dht.readTemperature();
    Serial.print("Temperature:");
    Serial.println(temperature);

    // Hora
    String hora = ntp.getFormattedTime();//Armazena na váriavel HORA, o horario atual.
    Serial.print("Hora: ");
    Serial.println(hora);

    if(!isnan(humidity) && !isnan(temperature)){

      //Push temperatura
      while(fail){
        Firebase.pushFloat("temperature2", temperature);
        Serial.print("Temperature failed = ");
        fail = Firebase.failed();
        Serial.println(fail);
      }

      fail = true;

      //Push Humidade
      while(fail){
        Firebase.pushFloat("humidity2", humidity);
        Serial.print("Humidity failed = ");
        fail = Firebase.failed();
        Serial.println(fail);
      }

      fail = true;

      //Push Hora
      while(fail){
        Firebase.pushString("time", hora);
        Serial.print("Time failed = ");
        fail = Firebase.failed();
        Serial.println(fail);
      }
      publishNewState = false;
  }

  if (humidity > 47){
    digitalWrite(LAMP_PIN, HIGH);
  } else {
    digitalWrite(LAMP_PIN, LOW);
  }
  delay(1000);
}
}
