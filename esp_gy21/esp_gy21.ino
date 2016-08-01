#include <ESP8266WiFi.h>
#include <MicroGear.h>
#include "SparkFun_Si7021_Breakout_Library.h"
#include <Wire.h>

#include "CMMC_Blink.hpp"
CMMC_Blink blinker;

const char* ssid     = "ESPERT-002"; // แก้ SSID
const char* password = "espresso";   // รหัส

#define APPID       "apinun"                    // App
#define KEY         "XgwasO4jRuYi3vf"           // key 
#define SECRET      "Q0VllgXTv9d23UEf8rQpqxz1U" //SECRET
#define ALIAS       "plugtest"                  //ALIAS 

WiFiClient client;
AuthClient *authclient;

float humidity = 0;
float temp = 0;

Weather sensor;

int timer = 0;
int relayPin = 15; //control relay pin

MicroGear microgear(client);

/* If a new message arrives, do this */
void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
  Serial.print("Incoming message --> ");
  msg[msglen] = '\0';
  Serial.println((char *)msg);
  String msg2 = String((char*)msg);

  if (msg2 == "ON") {
    digitalWrite(relayPin, HIGH);
    digitalWrite(LED_BUILTIN, LOW);
  }
  else if (msg2 == "OFF") {
    digitalWrite(relayPin, LOW);
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

void onFoundgear(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Found new member --> ");
  for (int i = 0; i < msglen; i++)
    Serial.print((char)msg[i]);
  Serial.println();
}

void onLostgear(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Lost member --> ");
  for (int i = 0; i < msglen; i++)
    Serial.print((char)msg[i]);
  Serial.println();
}

/* When a microgear is connected, do this */
void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
  //on led when Connected to NETPIE
  analogWrite(LED_BUILTIN, 0); //LED_BUILTIN use avctive Low to On
  /* Set the alias of this microgear ALIAS */
  microgear.setName(ALIAS);
}


void setup() {
  /* Add Event listeners */
  /* Call onMsghandler() when new message arraives */
  microgear.on(MESSAGE, onMsghandler);

  /* Call onFoundgear() when new gear appear */
  microgear.on(PRESENT, onFoundgear);

  /* Call onLostgear() when some gear goes offline */
  microgear.on(ABSENT, onLostgear);

  /* Call onConnected() when NETPIE connection is established */
  microgear.on(CONNECTED, onConnected);

  pinMode(relayPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  sensor.begin();
  blinker.init();
  Serial.begin(115200);
  Serial.println("Starting...");

  blinker.blink(50, LED_BUILTIN);
  delay(200);

  if (WiFi.begin(ssid, password)) {
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  blinker.blink(200, LED_BUILTIN);
  /* Initial with KEY, SECRET and also set the ALIAS here */
  microgear.init(KEY, SECRET, ALIAS);

  /* connect to NETPIE to a specific APPID */
  microgear.connect(APPID);

  // connected to netpie so turn off the led
  blinker.detach();
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  /* To check if the microgear is still connected */
  if (microgear.connected()) {
    // Serial.println("connected");

    /* Call this method regularly otherwise the connection may be lost */
    microgear.loop();

    if (timer >= 2000) {
      Serial.print("Publish... ");
      //******  read DHT sensor very 2sec
      float h = 0.00f;
      float t = 0.00f;

      h = sensor.getRH();

      t = sensor.getTemp();

      if (isnan(h) || isnan(t)) {
        Serial.println("Failed to read from DHT sensor!");
        //return;
      }
      else {
        Serial.print("Humidity: ");
        Serial.print(h);
        Serial.print(" %\t");
        Serial.print("Temperature: ");
        Serial.print(t);
        Serial.println(" *C ");

        /* Chat with the microgear named ALIAS which is myself */
        //microgear.chat("plug001/temp", (String)t);
        //microgear.chat("plug001/humid", (String)h);

        char topic_temp[MAXTOPICSIZE];
        char topic_humid[MAXTOPICSIZE];
        sprintf(topic_temp, "/gearname/%s/temp", ALIAS);
        sprintf(topic_humid, "/gearname/%s/humid", ALIAS);
        //retain message
        microgear.publish(topic_temp, String(t), true);
        microgear.publish(topic_humid, String(h), true);
      }
      timer = 0;
    }
    else timer += 100;
  }
  else {
    blinker.blink(100);
    Serial.println("connection lost, reconnect...");
    if (timer >= 5000) {
      microgear.connect(APPID);
      timer = 0;
      blinker.detach();
    }
    else timer += 100;
  }
  delay(100);
}
