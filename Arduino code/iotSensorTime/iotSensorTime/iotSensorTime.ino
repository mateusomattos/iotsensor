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
#define WIFI_SSID "OPORTOLIVING"
#define WIFI_PASSWORD "oporto22"

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
}


WiFiUDP udp;//Cria um objeto "UDP".
NTPClient ntp(udp, "a.st1.ntp.br", 0* 3600, 60000); //Cria um objeto "NTP" com as configurações.



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
  int principal = 1;

  // Hora
  String hora = ntp.getFormattedTime(); //Armazena na váriavel HORA, o horario atual.
  Serial.print("Hora: ");
  Serial.println(hora);
  
  // Apenas publique quando passar o tempo determinado ou na hora determinada
  //if(publishNewState){
  if(hora == "00:00:00" || hora == "01:00:00" || hora == "02:00:00" || hora == "03:00:00" || hora == "04:00:00" || hora == "05:00:00" || hora == "06:00:00" ||
      hora == "07:00:00" || hora == "08:00:00" || hora == "09:00:00" || hora == "10:00:00" || hora == "11:00:00" || hora == "12:00:00" || hora == "13:00:00" || 
      hora == "14:00:00" || hora == "15:00:00" || hora == "16:00:00" || hora == "17:00:00" || hora == "18:00:00" || hora == "19:00:00" || hora == "20:00:00" || 
      hora == "21:00:00" || hora == "22:00:00" || hora == "23:00:00"){
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
      if(principal == 1){
      //Push Hora
      while(fail){
        
        Firebase.pushString("time", hora);
        Serial.print("Time failed = ");
        fail = Firebase.failed();
        Serial.println(fail);
      }
      }
      publishNewState = false;
  }

  if (humidity > 47){
    digitalWrite(LAMP_PIN, HIGH);
  } else {
    digitalWrite(LAMP_PIN, LOW);
  }
  
}
delay(1000);
}
