#include <dht11.h>
#include <ArduinoMqttClient.h>
#include <WiFi101.h>
#include "wifi_credentials.h"

#define DHT11PIN 4  //pin collegato al DHT

dht11 DHT11;

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

char ssid[] = SECRET_SSID;
char pwd[] = SECRET_PASS;

//mqtt settings
const char broker[] = "test.mosquitto.org"; //ip del broker MQTT a cui mi collego
int port = 1883;
const char topicTemp[] = "temp_mia_api";
const char topicHum[] = "hum_mia_api"; 

float temp = 0.0;
float hum = 0.0;


void  setup(){
  Serial.begin(9600);

  //connessione alla rete
  Serial.print("Tentativo di connessione alla rete");
  Serial.println(ssid);

  while (WiFi.begin(ssid, pwd) != WL_CONNECTED) {
    //riprova
    Serial.print(".");
    delay(5000);
  }

  Serial.println("Sei connesso alla rete");
  Serial.println();
  //

  //connessione al broker mqtt
  Serial.print("Tentativo di connessione al broker MQTT: ");
  Serial.println(broker);

  while (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    Serial.println("Riprovo tra 5 secondi...");
    delay(5000);
  }

  Serial.println("Sei connesso al broker MQTT");
  //

  Serial.println();
}

void loop()
{
  //serve a mandare dei "keep-alive" per notificare il broker di non interrompere la connessione
  mqttClient.poll();

  //legge i dati dal sensore
  readDHT();
  //invia i dati al broker MQTT
  publishViaMQTT();
  
  delay(2000);

}

void readDHT() {
  int status = DHT11.read(DHT11PIN);
  Serial.print("Lettura DHT: ");
  Serial.println(status);

  if (status == DHTLIB_OK) {
    Serial.print("Umidità (%): ");
    hum = (float) DHT11.humidity;
    Serial.println(hum, 2);

    Serial.print("Temperatura  (C): ");
    temp = (float) DHT11.temperature;
    Serial.println(temp, 2);
  } else {
    Serial.println("Errore nella lettura del DHT11");
  }
  Serial.println();
}

void publishViaMQTT(){
  mqttClient.beginMessage(topicHum, true);  //true indica retained, serve a pubblicare il messaggio ai nuovi arrivati sul broker
  mqttClient.print(hum);
  mqttClient.endMessage();

  mqttClient.beginMessage(topicTemp, true);
  mqttClient.print(temp);
  mqttClient.endMessage();

  Serial.println();
}
