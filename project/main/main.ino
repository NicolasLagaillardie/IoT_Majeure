/** Sensing and publishing
 *  Xavier Serpaggi <serpaggi@emse.fr>
 *
 *  Two sensors plugged to an Arduino MKR 1010
 *  - Light : TEMT6000 on Analog pin A0
 *  - Temperature/Humidity : DHT22 or DS1820, data pin on GPIO pin #4
 *  Values are sent to an MQTT broker.
 *
 * Required libraries (sensing):
 *  - DallasTemperature
 *  - OneWire (Included in the latest versions of DallasTemperature)
 *  - Adafruit Unified Sensor
 *  - DHT Sensor Library (by Adafruit)
 *
 * Required libraries (communication & MQTT):
 *  - WiFiNINA
 *  - MQTT (by Joel Gaehwiler)
 */

#include <string.h>

#include <Wire.h>
#include <SPI.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <WiFiNINA.h>
#include <MQTT.h>

/* In which pins are my sensors plugged? */
#define TMP 4   // DHT22 Signal pin


/* We need a DHT object to address the sensor. */
DHT dht(TMP, DHT22) ; // pin: TMP, model: DHT22


/* We need objects to handle:
 *  1. WiFi connectivity
 *  2. MQTT messages
 */
WiFiClient wifi_client ;
MQTTClient mqtt_client ;

/* And associated variables to tell:
 *  1. which WiFi network to connect to
 *  2. what are the MQTT broket IP address and TCP port
 */
const char* wifi_ssid     = "lab-iot-1";
const char* wifi_password = "lab-iot-1";

const char* mqtt_host = "m21.cloudmqtt.com" ;
const uint16_t mqtt_port = 19316;
const char* mqtt_user="ddrcrasn";
const char* mqtt_password="e7czA49tmqUA";



/* Some variables to handle measurements. */
float tmp ;
boolean first_time ;

uint32_t t0, t ;


/* 'topic' is the string representing the topic on which messages
 *  will be published.
 *
 * TODO: You need to adapt it by setting:
 *  1. 'surname' to your surname
 *  2. 'ID' to a unique ID chosen in the class with other students
 */
String topic = "sensor/temp/1" ;


/* Time between two sensings and values sent. */
#define DELTA_T 2000


/* ################################################################### */
void setup() {
  Serial.begin(9600) ;  // monitoring via Serial is always nice when possible
  delay(100) ;
  Serial.println("Initializing...\n") ;

  dht.begin() ;

  WiFi.begin(wifi_ssid, wifi_password) ;
  mqtt_client.begin(mqtt_host, mqtt_port, wifi_client) ;

  tmp = -1.0 ;

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

    tmp = getTmp() ;

    sendValues() ;
  }
}


/* ------------------------------------------------------------------- */
double getTmp()
{
  return dht.readTemperature() ;
}

/* ################################################################### */
/* This function handles the connection/reconnection to
 * the MQTT broker.
 * Each time a connection or reconnection is made, it
 * publishes a message on the topic+"/HELLO" containing
 * the number of milli seconds since uController start.
 */
void reconnect() {
  Serial.print("Connecting to ");
  Serial.print(wifi_ssid);
  Serial.print("\n") ;

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

  while ( !mqtt_client.connect("2", mqtt_user,mqtt_password) ) {
    delay(500);
    Serial.println(".");
    delay(500);
  }
  Serial.println("MQTT: connected") ;
  Serial.print("MQTT: root topic is \"") ;
  Serial.print(topic) ;
  Serial.println("\"") ;

  /*mqtt_client.publish(String(topic).c_str(), String(tmp).c_str()) ;*/

  /* If you want to subscribe to topics, you have to do it here. */
}

/* ################################################################### */
/* This function handles the sending of all the values
 * collected by the sensors.
 * Values are sent on a regular basis (every DELTA_T_SEND_VALUES
 * milliseconds).
 */
void sendValues() {
  if ( mqtt_client.connected() ) {
    if ( tmp != -1 )
      mqtt_client.publish(String(topic).c_str(), String(tmp).c_str()) ;
  }
}
