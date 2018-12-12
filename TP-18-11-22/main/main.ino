/** Sensing and publishing
    Xavier Serpaggi <serpaggi@emse.fr>

    Two sensors plugged to an Arduino MKR 1010
    - Light : TEMT6000 on Analog pin A0
    - Temperature/Humidity : DHT22 or DS1820, data pin on GPIO pin #4
    Values are sent to an MQTT broker.

   Required libraries (sensing):
    - DallasTemperature
    - OneWire (Included in the latest versions of DallasTemperature)
    - Adafruit Unified Sensor
    - DHT Sensor Library (by Adafruit)

   Required libraries (communication & MQTT):
    - WiFiNINA
    - MQTT (by Joel Gaehwiler)
*/

#include <string.h>

#include <Wire.h>
#include <SPI.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>

#include <WiFiNINA.h>
#include <MQTT.h>

/* In which pins are my sensors plugged? */
#define LUM A0  // TEMT6000 Signal pin
#define TMP 4   // DHT22 Signal pin


/* We need a DHT object to address the sensor. */
DHT dht(TMP, DHT22) ; // pin: TMP, model: DHT22


/* We need objects to handle:
    1. WiFi connectivity
    2. MQTT messages
*/
WiFiClient wifi_client ;
MQTTClient mqtt_client ;

/* And associated variables to tell:
    1. which WiFi network to connect to
    2. what are the MQTT broket IP address and TCP port
*/
const char* wifi_ssid     = "gr5";
const char* wifi_password = "92b8276658310";

const char* mqtt_host = "m21.cloudmqtt.com" ;
const uint16_t mqtt_port =  13505 ;
const char* mqtt_username = "lfdwmgnz";
const char* mqtt_password = "e6jp9_8_xVcK";

/* Some variables to handle measurements. */
int tmp ;
int lum ;
int hmdt ;
boolean first_time ;

uint32_t t0, t ;


/* 'topic' is the string representing the topic on which messages
    will be published. */



String topic = "/ITM/enviroment/" ;


/* Time between two sensings and values sent. */
#define DELTA_T 2000


/* ################################################################### */
void setup() {
  Serial.begin(9600) ;  // monitoring via Serial
  delay(100) ;
  Serial.println("Initializing...\n") ;

  pinMode(LED_BUILTIN, OUTPUT) ;
  digitalWrite(LED_BUILTIN, LOW) ;  // switch LED off

  dht.begin() ;

  WiFi.begin(wifi_ssid, wifi_password) ;
  mqtt_client.begin(mqtt_host, mqtt_port, wifi_client) ;
  mqtt_client.onMessage(callback) ;

  tmp = lum = hmdt = -1.0 ;

  first_time = true ;

  // Time begins now!
  t0 = t = millis() ;




}








/* ################################################################### */
void loop() {
  /* We try to connect to the broker and launch the client loop.
  */
  mqtt_client.loop() ;
  if ( ! mqtt_client.connected() ) {
    reconnect() ;
  }

  /* Values are read from sensors at fixed intervals of time.
  */
  // ===================================================
  t = millis() ;
  if ( first_time || (t - t0) >= DELTA_T ) {
    t0 = t ;
    first_time = false ;

    lum = getLum() ;
    tmp = getTmp() ;
    hmdt = getHmdt() ;



    DynamicJsonDocument doc;
    String data = "{\"Temperature *C\":\"";
    data += dht.readTemperature();
    data += "\",\"Humidity\":\"";
    data += dht.readHumidity();
    data += "\"}";
    deserializeJson(doc, data);
    JsonObject root = doc.as<JsonObject>();



    if ( mqtt_client.publish("/ITM/enviroment/", data) == true) {
      Serial.println("Success sending message");
    } else {
      Serial.println("Error sending message");
    }





    sendValues() ;
  }
}


/* ------------------------------------------------------------------- */
double getLum()
{
  double acc = 0 ;
  uint8_t n_val ;

  for ( n_val = 0 ; n_val < 10 ; n_val++ ) {
    acc += analogRead(LUM) ;
    delay(5) ;
  }

  return acc / n_val ;
}


/* ------------------------------------------------------------------- */
double getTmp()
{
  return dht.readTemperature() ;
}


/* ------------------------------------------------------------------- */
double getHmdt()
{
  return dht.readHumidity() ;
}

/* ################################################################### */
/* This function handles the connection/reconnection to
   the cloud MQTT broker.*/






















void reconnect() {
  Serial.print("Connecting to ");
  Serial.print(wifi_ssid);
  Serial.print("\n") ;

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, HIGH) ;
    delay(500);
    digitalWrite(LED_BUILTIN, LOW) ;
    Serial.print(".");
    delay(500);
  }

  Serial.print("\n");
  Serial.print("WiFi connected\n");
  Serial.print("IP address: \n");
  Serial.print(WiFi.localIP());
  Serial.print("\n") ;


  Serial.print("MQTT: trying to connect to host ") ;
  Serial.print(mqtt_host) ;
  Serial.print(" on port ") ;
  Serial.print(mqtt_port) ;
  Serial.println(" ...") ;

  while (!mqtt_client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (mqtt_client.connect("MQTTClient", mqtt_username, mqtt_password)) {

      Serial.println("connected");

    } else {

      Serial.print("failed with state ");
      //      Serial.print(client.state());
      delay(2000);

    }
  }



  mqtt_client.publish(String(topic).c_str(), String(millis())) ;

  /* If you want to subscribe to topics, you have to do it here. */
}

/* ################################################################### */
/* This function handles the sending of all the values
   collected by the sensors.
   Values are sent on a regular basis (every DELTA_T_SEND_VALUES
   milliseconds).
*/
void sendValues() {
  if ( mqtt_client.connected() ) {
    if ( lum != -1 )
      mqtt_client.publish(String(topic).c_str()) ;
    //    if ( tmp != -1 )
    //      mqtt_client.publish(String(topic + "/TEMP").c_str(), String(tmp).c_str()) ;
    //    if ( hmdt != -1 )
    //      mqtt_client.publish(String(topic + "/HMDT").c_str(), String(hmdt).c_str()) ;
  }
}


/* ################################################################### */
/* The main callback to listen to topics.
*/
void callback(String &intopic, String &payload)
{
  /* There's nothing to do here ... as long as the module
      cannot handle messages.
  */
  Serial.println("incoming: " + topic + " - " + payload);
}
