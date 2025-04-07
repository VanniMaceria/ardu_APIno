#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <WiFi.h>
#include <ArduinoMqttClient.h>
#include "wifi_credentials.h"
#include <ArduinoJson.h>

#define DHT_PIN 16  //pin collegato al DHT
#define DHT_TYPE DHT11  
#define RED_LED_PIN 13  
#define GREEN_LED_PIN 14  
#define BLUE_LED_PIN 15
#define RGB_RED_PIN 19
#define RGB_GREEN_PIN 18
#define RGB_BLUE_PIN 17

DHT dht(DHT_PIN, DHT_TYPE);

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

char ssid[] = SECRET_SSID;
char pwd[] = SECRET_PASS;

//MQTT settings
const char broker[] = "test.mosquitto.org";
int port = 1883;
const char topicTemp[] = "temp_mia_api";
const char topicHum[] = "hum_mia_api"; 
const char topicbrightness[] = "led_mia_api";
const char topicRGB_led[] = "RGB_led_mia_api";

float temp = 0.0;
float hum = 0.0;

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(DHT_PIN, INPUT);  //pin DHT come input
  //imposta i pin dei LED RGB come uscita
  pinMode(RED_LED_PIN, OUTPUT);  
  pinMode(GREEN_LED_PIN, OUTPUT);  
  pinMode(BLUE_LED_PIN, OUTPUT); 

  pinMode(RGB_RED_PIN, OUTPUT);  
  pinMode(RGB_GREEN_PIN, OUTPUT);  
  pinMode(RGB_BLUE_PIN, OUTPUT); 

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

  mqttClient.subscribe(topicbrightness, 1);  //sottoscrivi al topic di accensione led con QoS-1
  mqttClient.subscribe(topicRGB_led, 1);
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
  //legge il topic e il payload
  String topic = mqttClient.messageTopic();
  String payload = mqttClient.readString();
  
  Serial.print("Messaggio ricevuto sul topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  Serial.println(payload);

  StaticJsonDocument<200> doc;  //oggetto JSON
  DeserializationError error = deserializeJson(doc, payload); //parsing del JSON

  //controlla il topic
  if (topic == topicbrightness) {
    if (error) {
      Serial.print("Errore nel parsing JSON: ");
      Serial.println(error.c_str());
      return;
    }

    //estraggo i valori dal JSON
    int brightness = doc["brightness"];
    String color = doc["color"];

    //limita il valore della luminosità tra 0 e 255
    int brightness_constrained = constrain(brightness, 0, 255);

    //accende il LED giusto in base al colore ricevuto
    if (color.equalsIgnoreCase("red")) {
        analogWrite(RED_LED_PIN, brightness_constrained);
      } else if (color.equalsIgnoreCase("green")) {
        analogWrite(GREEN_LED_PIN, brightness_constrained);
      } else if (color.equalsIgnoreCase("blue")) {
        analogWrite(BLUE_LED_PIN, brightness_constrained);
      } else {
        Serial.println("Colore non riconosciuto!");
      }

    Serial.print("Luminosità del LED ");
    Serial.print(color);
    Serial.print(" settata a: ");
    Serial.println(brightness_constrained);

  } else if(topic == topicRGB_led){
      if (error) {
        Serial.print("Errore nel parsing JSON: ");
        Serial.println(error.c_str());
        return;
      }

      //constrain(doc["r"] | 0, 0, 255) -> per 'r' prende il valore o usa 0 se manca e lo limita tra 0 e 255
      analogWrite(RGB_RED_PIN, (int)constrain(doc["r"] | 0, 0, 255));
      analogWrite(RGB_GREEN_PIN, (int)constrain(doc["g"] | 0, 0, 255));
      analogWrite(RGB_BLUE_PIN, (int)constrain(doc["b"] | 0, 0, 255));

  }
}

