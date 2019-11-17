#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <Ticker.h>
#include "DHT.h"

// Set these to run example.
#define FIREBASE_HOST "iotsensor-cc9bd.firebaseio.com"
#define FIREBASE_AUTH "AXr5h8eoQmofPVUyM6pOZTBgNn30oAmTBKuZUVaP"

//#define WIFI_SSID "Vodafone-EBCF85"
//#define WIFI_PASSWORD "11ACAAE8F8"
#define WIFI_SSID "AndroidAP"
#define WIFI_PASSWORD "haks8425"

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

void setup() {
  Serial.begin(115200);

  setupPins();
  setupWifi();    

  setupFirebase();

  // Registra o ticker para publicar de tempos em tempos
  ticker.attach_ms(PUBLISH_INTERVAL, publish);
}

void loop() {

  float humidity = 0;
  
  // Apenas publique quando passar o tempo determinado
  if(publishNewState){
    Serial.println("Publish new State");
    // Obtem os dados do sensor DHT 
    humidity = dht.readHumidity();
    Serial.print("Humidity:");
    Serial.println(humidity);
    float temperature = dht.readTemperature();
    Serial.print("Temperature:");
    Serial.println(temperature);
    if(!isnan(humidity) && !isnan(temperature)){
      // Manda para o firebase
      String string1 = Firebase.pushFloat("temperature", temperature);
      Serial.print("Temperature failed = ");
      Serial.println(Firebase.failed());
      Serial.print("Key=");
      Serial.println(string1);
      Firebase.pushFloat("humidity", humidity);
      Serial.print("Humidity failed = ");
      Serial.println(Firebase.failed());
      publishNewState = false;
      if(Firebase.failed()){
        Serial.println(Firebase.error());}
    }else{
      Serial.println("Error Publishing");
    }
  }

  if (humidity > 47){
    digitalWrite(LAMP_PIN, HIGH);
  } else {
    digitalWrite(LAMP_PIN, LOW);
  }
    
  

  // Verifica o valor do sensor de presença
  // LOW sem movimento
  // HIGH com movimento
  //int presence = digitalRead(PRESENCE_PIN);  
  //Firebase.setBool("presence", presence == HIGH);

  // Verifica o valor da lampada no firebase 
  //bool lampValue = Firebase.getBool("lamp");
  //digitalWrite(LAMP_PIN, lampValue ? HIGH : LOW);
  //publishNewState = false;
  delay(1000);
}