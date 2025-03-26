#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <WiFi.h>
#include <ArduinoMqttClient.h>
#include "wifi_credentials.h"

#define DHTPIN 16  //pin collegato al DHT
#define DHTTYPE DHT11  
#define LEDPIN 13  //pin del LED

DHT dht(DHTPIN, DHTTYPE);

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

char ssid[] = SECRET_SSID;
char pwd[] = SECRET_PASS;

//MQTT settings
const char broker[] = "test.mosquitto.org";
int port = 1883;
const char topicTemp[] = "temp_mia_api";
const char topicHum[] = "hum_mia_api"; 
const char topicVoltage[] = "voltaggio_led_mia_api";

float temp = 0.0;
float hum = 0.0;

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(DHTPIN, INPUT);
  pinMode(LEDPIN, OUTPUT);  //imposta il pin del LED come uscita

  // Connessione alla rete WiFi
  Serial.print("Connessione a ");
  Serial.println(ssid);
  WiFi.begin(ssid, pwd);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\nConnesso alla rete WiFi");

  // Connessione al broker MQTT
  Serial.print("Connessione al broker MQTT...");
  while (!mqttClient.connect(broker, port)) {
    Serial.println("Connessione fallita! Riprovo tra 5 secondi...");
    delay(5000);
  }
  Serial.println("Connesso al broker MQTT!");

  //sottoscrizione al topic per il voltaggio del LED
  mqttClient.subscribe(topicVoltage, 1);  //sottoscrivi al topic luminosità con QoS-1
  mqttClient.onMessage(onMessageReceived);  //imposta la callback per gestire i messaggi
}

void loop() {
  mqttClient.poll(); //mantiene la connessione MQTT attiva e gestisce i messaggi

  //legge i dati dal sensore
  readDHT();
  
  //invia i dati al broker MQTT
  publishViaMQTT();
  
  delay(5000);
}

void readDHT() {
  hum = dht.readHumidity();
  temp = dht.readTemperature();

  Serial.print("Umidità: ");
  Serial.print(hum);
  Serial.print(" %");
  Serial.print("Temperatura: ");
  Serial.print(temp);
  Serial.println(" °C");
}

void publishViaMQTT() {
  mqttClient.beginMessage(topicHum, true);
  mqttClient.print(hum);
  mqttClient.endMessage();

  mqttClient.beginMessage(topicTemp, true);
  mqttClient.print(temp);
  mqttClient.endMessage();
}

void onMessageReceived(int messageSize) {
  //legge il messaggio ricevuto
  String topic = mqttClient.messageTopic();
  String payload = mqttClient.readString(); 
  Serial.print("Messaggio ricevuto sul topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  Serial.println(payload);

  //controlla il topic e il valore ricevuto per il voltaggio
  if (topic == topicVoltage) {
    int brightness = payload.toInt(); //converte il payload in intero (luminosità)

    //limita la luminosità a un valore tra 0 e 255
    brightness = constrain(brightness, 0, 255);

    //imposta la luminosità del LED tramite PWM
    analogWrite(LEDPIN, brightness);
    Serial.print("Luminosità del LED settata a: ");
    Serial.println(brightness);
  }
}
