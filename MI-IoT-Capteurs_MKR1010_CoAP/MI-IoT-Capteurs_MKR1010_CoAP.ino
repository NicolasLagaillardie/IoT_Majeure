/** Sensing
 *  Xavier Serpaggi <serpaggi@emse.fr>
 *  Maxime Lefrançois <maxime.lefrancois@emse.fr>
 *
 *  Two sensors plugged to an Arduino MKR 1010
 *  - Light : TEMT6000 on Analog pin A0
 *  - Temperature/Humidity : DHT22 or DS1820, data pin on GPIO pin #4
 *  Values are sent via Serial communication.
 *  
 * Required libraries:
 *  - DallasTemperature
 *  - OneWire (Included in the latest versions of DallasTemperature)
 *  - Adafruit Unified Sensor
 *  - DHT Sensor Library (by Adafruit)
 *  - Coap Simple library
 */

#include <string.h>
#include <stdio.h>

#include <Wire.h>
#include <SPI.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>


#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <coap.h>

/* In which pins are my sensors plugged? */
#define LUM A0  // TEMT6000 Signal pin
#define TMP 4   // DHT22 Signal pin


/* We need a DHT object to address the sensor. */
DHT dht(TMP, DHT22) ; // pin: TMP, model: DHT22

/* We need objects to handle:
 *  1. WiFi connectivity
 *  2. MQTT messages
 */
WiFiClient wifi_client ;
WiFiUDP udp;
Coap coap(udp);

/* And associated variables to tell:
 *  1. which WiFi network to connect to
 *  2. what are the MQTT broket IP address and TCP port
 */
const char* wifi_ssid     = "lab-iot-1";
const char* wifi_password = "lab-iot-1";
int status = WL_IDLE_STATUS; // the Wifi radio's status


/* Some variables to handle measurements. */
int tmp ;
int lum ;
int hmdt ;
boolean first_time ;
char data[50];

uint32_t t0, t ;


/* Time between two sensings. */
#define DELTA_T 2000


// LED STATE
bool LEDSTATE;

// CoAP server endpoint URL
void callback_core(CoapPacket &packet, IPAddress ip, int port) {
  Serial.println("Core");

  char* message = "</light>,</sensor/lum>,</sensor/tmp>"; // AUGMENT HERE, CONSULT RFC 6690
  int payloadlen = sizeof(message);
  // send response
  coap.sendResponse(ip, port, packet.messageid, message, strlen(message), COAP_CONTENT, COAP_APPLICATION_LINK_FORMAT, NULL, 0);
}

void callback_lum(CoapPacket &packet, IPAddress ip, int port) {
  double t = getLum();
  Serial.print("sensor/lum");
  Serial.println(t);
  Serial.println( String(getLum()).c_str());

  String message =  "{ 'thevalueis': " + String(getLum()) + "}";
  char msg[message.length()+1];
  strcpy(msg, message.c_str());

  int payloadlen = sizeof(message);
  // send response
  coap.sendResponse(ip, port, packet.messageid, msg, strlen(msg), COAP_CONTENT, COAP_APPLICATION_JSON, NULL, 0);
}

void callback_light(CoapPacket &packet, IPAddress ip, int port) {
  
  // send response
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;
  
  String message(p);

  switch(packet.code) {
    case COAP_GET:
      Serial.println("Received GET at </light> with message: " + message);
      break;
    case COAP_POST:
      Serial.println("Received POST at </light> with message: " + message);
      if (message.equals("0")) {
          Serial.println("turn off LED");
      } else if(message.equals("1")) {
          Serial.println("turn on LED");
      }
      break;
    case COAP_PUT:
      Serial.println("Received PUT at </light> with message: " + message);
      break;
    case COAP_DELETE:
      Serial.println("Received DELETE at </light> with message: " + message);
      break;
    default:
      Serial.println("Received some code at </light> with message: " + message);
  }

  coap.sendResponse(ip, port, packet.messageid, "ok");
}











void callback_tmp(CoapPacket &packet, IPAddress ip, int port) {
  double t = getTmp();
  Serial.print("sensor/tmp");
  Serial.println(t);
  Serial.println( String(getTmp()).c_str());

  String message =  "{ 'thevalueis': " + String(getTmp()) + "}";
  char msg[message.length()+1];
  strcpy(msg, message.c_str());

  int payloadlen = sizeof(message);
  // send response
  coap.sendResponse(ip, port, packet.messageid, msg, strlen(msg), COAP_CONTENT, COAP_APPLICATION_JSON, NULL, 0);
}

void callback_hmdt(CoapPacket &packet, IPAddress ip, int port) {
  double t = getHmdt();
  Serial.print("sensor/hmdt");
  Serial.println(t);
  Serial.println( String(getHmdt()).c_str());

  String message =  "{ 'thevalueis': " + String(getHmdt()) + "}";
  char msg[message.length()+1];
  strcpy(msg, message.c_str());

  int payloadlen = sizeof(message);
  // send response
  coap.sendResponse(ip, port, packet.messageid, msg, strlen(msg), COAP_CONTENT, COAP_APPLICATION_JSON, NULL, 0);
}






/* ################################################################### */
void setup() {
  Serial.begin(9600) ;  // monitoring via Serial is always nice when possible
  delay(100) ;
  Serial.println("Initializing...\n") ;

  pinMode(LED_BUILTIN, OUTPUT) ;
  digitalWrite(LED_BUILTIN, LOW) ;  // switch LED off

  dht.begin() ;
 
  while (status != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(wifi_ssid);
    Serial.print("\n") ;
    status = WiFi.begin(wifi_ssid, wifi_password);
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


  tmp = lum = hmdt = -1.0 ;

  // LED State
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
  LEDSTATE = true;
  
  // add server url endpoints.
  // can add multiple endpoint urls.
  // exp) coap.server(callback_switch, "switch");
  //      coap.server(callback_env, "env/temp");
  //      coap.server(callback_env, "env/humidity");
  Serial.println("Setup Callbacks");
  coap.server(callback_core, ".well-known/core");
  coap.server(callback_light, "light");
  coap.server(callback_lum, "sensor/lum");
  coap.server(callback_tmp, "sensor/tmp");
  coap.server(callback_tmp, "sensor/hmdt");

  // start coap server/client
  coap.start();
  
}


/* ################################################################### */
void loop() {
  coap.loop();
}
/*
if you change LED, req/res test with coap-client(libcoap), run following.
coap-client -m get coap://(arduino ip addr)/light
coap-client -e "1" -m put coap://(arduino ip addr)/light
coap-client -e "0" -m put coap://(arduino ip addr)/light
*/

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
