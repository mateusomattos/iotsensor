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
//#define WIFI_SSID "LGG6"
//#define WIFI_PASSWORD "12345678"

#define WIFI_SSID "OPORTOLIVING"
#define WIFI_PASSWORD "oporto22"


#define LAMP_PIN D3
#define PRESENCE_PIN D4
#define DHT_PIN D5
#define DHTTYPE DHT11

// Publique a cada 5 min
#define PUBLISH_INTERVAL 1000*10*1

//intervalos de tempo para publicacao
#define HORA
#define MINUTO
#define SEGUNDO


int first = 1;
String horaProximo;
String proximaMedicao;
int medirFlag = 0;
bool medicaoAgora = false;
String hora;

DHT dht(DHT_PIN, DHTTYPE);


void setupPins(){
  pinMode(LAMP_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);  
  digitalWrite(LAMP_PIN, LOW);
  dht.begin();  
}

void setupWifi(){
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH); 
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH); 
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH); 
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
}

void setupFirebase(){
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}


WiFiUDP udp;//Cria um objeto "UDP".
NTPClient ntp(udp, "pool.ntp.org"); //Cria um objeto "NTP" com as configurações.



void setup() {
  Serial.begin(115200);
  
  setupPins();
  digitalWrite(LED_BUILTIN, LOW);
  setupWifi();    

  setupFirebase();

  ntp.begin();//Inicia o NTP.
  ntp.forceUpdate();//Força o Update.

}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);

  if(WiFi.status() != WL_CONNECTED){
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("connecting");
    while (WiFi.status() != WL_CONNECTED) {
        digitalWrite(LED_BUILTIN, HIGH); 
        delay(200);
        digitalWrite(LED_BUILTIN, LOW);
        delay(200);
  }
    
  }

  float humidity = 0;
  bool fail = true;
  int principal = 1;

  //medicaoAgora = Firebase.getBool("medir");
  // Hora
  hora = ntp.getFormattedTime(); //Armazena na váriavel HORA, o horario atual.
  Serial.print("Hora: ");
  Serial.println(hora);

  String horaAgora = hora.substring(0,2);
  String minutoAgora = hora.substring(3,5);
  String segundoAgora = hora.substring(6,9);

  int horaAgoraInt = horaAgora.toInt();
  int minutoAgoraInt = minutoAgora.toInt();
  int segundoAgoraInt = segundoAgora.toInt();

  if(proximaMedicao == hora){
    medirFlag = 1;
  }

  if(horaAgoraInt == 23){
    horaProximo = "00";
  }

  else{
    horaAgoraInt++;

    if(horaAgoraInt < 10){
      horaProximo = "0"+String(horaAgoraInt);
    }
    
    else{
      horaProximo = String(horaAgoraInt);
    }
  }

  proximaMedicao = horaProximo+":00:00";
  Serial.println(proximaMedicao);


  
  // Apenas publique quando passar o tempo determinado ou na hora determinada
  if(medirFlag == 1 || medicaoAgora == true){
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
        digitalWrite(LED_BUILTIN, LOW); 
          delay(100);
          digitalWrite(LED_BUILTIN, HIGH);
          delay(100);
      }

      fail = true;

      //Push Humidade
      while(fail){
        Firebase.pushFloat("humidity2", humidity);
        Serial.print("Humidity failed = ");
        fail = Firebase.failed();
        Serial.println(fail);
        digitalWrite(LED_BUILTIN, LOW); 
          delay(100);
          digitalWrite(LED_BUILTIN, HIGH);
          delay(100);
      }

      fail = true;
      if(principal == 1){
        //Push Hora
        while(fail){
          
          Firebase.pushString("time", hora);
          Serial.print("Time failed = ");
          fail = Firebase.failed();
          Serial.println(fail);
          digitalWrite(LED_BUILTIN, LOW); 
          delay(100);
          digitalWrite(LED_BUILTIN, HIGH);
          delay(100);
        }
      }
      medirFlag = 0;
      Firebase.setBool("medir", false);

  }
  
}

delay(500);
}
