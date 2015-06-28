
#include <ESP8266WiFi.h>
#include <Wire.h>  
#define DEBUG
#define DEBUG_PRINTER Serial
#ifdef DEBUG
#define DEBUG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
#define DEBUG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
#define DEBUG_PRINT(...) {}
#define DEBUG_PRINTLN(...) {}
#endif
////////////////////////////////เป็นส่วนตัวแปลของ Si7005
int dataHighAddress = 0x01;
int dataLowAddress = 0x02;
byte data = 0xff;
byte dataH;
byte dataL;
int t;
int h;
float Temperature, Humidity; 
///////////////////////////////
const char* ssid     = "CMMC.47";
const char* pass     = "guestnetwork";

float temp;
void connectWifi();
void reconnectWifiIfLinkDown();
void uploadThingsSpeak(float t,float H);
 
void setup() {
    Serial.begin(115200);
    Wire.begin();  
    delay(10);
    connectWifi();
    delay(100);                //waiting sensor start
}
 
void loop() {
    Si7005_v1();
    uploadThingsSpeak(Temperature,Humidity);
    delay(200);
    reconnectWifiIfLinkDown();
    
}
 
void reconnectWifiIfLinkDown() {
    if (WiFi.status() != WL_CONNECTED) {
        DEBUG_PRINTLN("WIFI DISCONNECTED");
        connectWifi();
    }
}
 
void connectWifi() {
    DEBUG_PRINTLN();
    DEBUG_PRINTLN();
    DEBUG_PRINT("Connecting to ");
    DEBUG_PRINTLN(ssid);
 
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        DEBUG_PRINT(".");
    }
 
    DEBUG_PRINTLN("");
    DEBUG_PRINTLN("WiFi connected");
    DEBUG_PRINTLN("IP address: ");
    DEBUG_PRINTLN(WiFi.localIP());
}
void uploadThingsSpeak(float t,float H) {
    static const char* host = "api.thingspeak.com";   ///
    static const char* apiKey = "O9RCIHUIEPSY1NU9";   /// ตรงนี้ต้องแก้apiKeyเป็นของตัวเองนะครับ
 
    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
        DEBUG_PRINTLN("connection failed");
        return;
    }
 
    // We now create a URI for the request
    String url = "/update/";
    //  url += streamId;
    //-----------------------------------------------
    url += "?key=";
    url += apiKey;
    url += "&field1=";
    url += t;
    //-----------------------------------------------
    url += "?key=";
    url += apiKey;
    url += "&field2=";
    url += H;
//    url += "&field4=";
//    url += h;    
    //-----------------------------------------------
    
    
    
    DEBUG_PRINT("Requesting URL: ");
    DEBUG_PRINTLN(url);
 
    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
}
void Si7005_v1(){                   
   Wire.beginTransmission(0x40);
  Wire.write(0x03);
  Wire.write(0x11);
  Wire.endTransmission();
  delay(100);
  Wire.beginTransmission(0x40);
  Wire.write(0x00);
  Wire.requestFrom(0x40, 3);
  while (Wire.available())
  {
    data = Wire.read();
    t = (Wire.read() << 8);
    t |= Wire.read();
    t = t >> 2;
  }
  Wire.endTransmission();
  Temperature = (t / 32.00) - 50;
  Serial.print( "Temperature = " );
  Serial.println( Temperature );

  // Start the Humidity sensor
  Wire.beginTransmission(0x40);
  Wire.write(0x03);
  Wire.write(0x01);
  Wire.endTransmission();
  delay(100);
  Wire.beginTransmission(0x40);
  Wire.write(0x00);
  Wire.requestFrom(0x40, 3);
  while (Wire.available())
  {
    data = Wire.read();
    h = (Wire.read() << 8);
    h |= Wire.read();
    h= h >> 4;
    Wire.endTransmission();
    Humidity = (h / 16.00) - 24;
    Serial.print( "Humidity = " );
    Serial.println( Humidity );
    delay(100);
  }
  }



 

