#include <stdint.h>
#include <stdlib.h>

#include <WiFi.h> //<ESP8266WiFi.h>
#include <WebServer.h> //<ESP8266WebServer.h>
#include <WiFiAP.h>
#include <ESPmDNS.h> //<ESP8266mDNS.h> //
#include <ArduinoJson.h>

#include <EEPROM.h>
#include <Wire.h>
#include <Ticker.h>
#include "page1.h"
#include "page2.h"

typedef struct {
  char tag;
  char ssid[10];
  char pass[10];
  char sw1;
} myIoT;

const myIoT defaultObj = { 0, "Default\0", "1234567\0", 0};
myIoT myIoTObj;

void readObj(myIoT *obj);
void writeObj(myIoT *obj);

void ICACHE_RAM_ATTR keyEvent(void);
WebServer serverHTTP(80);

void chkUserData(myIoT *obj);

//const int keyIn    = 0; // GPIO16---D0 of NodeMCU
//const int GreenLed = 12; // GPIO12-- D6 of NodeMCU
const int RedLed   = 4; // GPIO13-- D7 of NodeMCU
//const int Relay    = 4;  // GPIO15-- D8 of NodeMCU

volatile bool intfired=false;

void setup() {
  myIoT *obj;
  uint64_t espChipID = ESP.getEfuseMac(); //ESP.getChipId();; 
  char   host[12] = "myiot-";
  char   cmac[6]  = "";

  pinMode     (RedLed,OUTPUT);
  readObj(&myIoTObj);
  if      (myIoTObj.sw1 == 0)   { digitalWrite(RedLed, LOW); }
  else                          { digitalWrite(RedLed, HIGH); }
  
  sprintf(cmac,"%2x",espChipID);
  strncat(host, cmac, 2);  
  
  Serial.begin(115200);  
  Serial.println("---------------");

  Serial.printf("MAC Address : %6X\n\r",espChipID);
  Serial.printf("HOST Name   : %s\n\r",host);
  WiFi.mode(WIFI_STA);
  WiFi.begin(myIoTObj.ssid, myIoTObj.pass);
  
  char mode = 0;
  int cnt = 0;
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (cnt++ == 10) { 
      mode = 1; break; 
    }    
  }

 if ( mode == 0 ) {
   Serial.printf("\nHost name : %s\n\r",host);
   Serial.printf("SSID      : %s\n\r",myIoTObj.ssid);  
   Serial.printf("Connecting %s by STA Mode, IP:",host); 
   Serial.println(WiFi.localIP());
  }
  else {
    WiFi.mode(WIFI_AP);
    Serial.println("\n\rSetting soft-AP configuration ... ");
   // Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
    if ( WiFi.softAP(host) == true ) { // true == ready
      IPAddress myIP = WiFi.softAPIP();
      Serial.printf("Connecting %s by AP Mode, IP:",host);
      Serial.println(myIP);
   }
  
  }
  chkUserData(&myIoTObj);
  
  //if (MDNS.begin(host)) {
  //  Serial.println("mDNS responder started");
  //}

  if (!MDNS.begin(host)) {
        Serial.println("Error setting up MDNS responder!");
        while(1){
            delay(1000);
        }
  }
  serverHTTP.on("/",        handleRoot);
  serverHTTP.on("/next"   , handlePage1);
  serverHTTP.on("/update", updateEvent);
  serverHTTP.on("/get"    ,   getEvent); 
  serverHTTP.on("/reset",   resetEvent);
  serverHTTP.on("/sw1",       sw1Event);
  serverHTTP.on("/sw1Get", sw1GetEvent);

  serverHTTP.begin();
  Serial.println("Server listing");
  delay(500);   
}

void ICACHE_RAM_ATTR keyEvent(void) {
    intfired=true;
 
  DynamicJsonDocument doc(64);
  char ss[4];
  if      (myIoTObj.sw1 == 0 ) myIoTObj.sw1 = 1;
  else                         myIoTObj.sw1 = 0;
  if      (myIoTObj.sw1 == 0)   { digitalWrite(RedLed, LOW); }
  else                          { digitalWrite(RedLed, HIGH); }
  sprintf(ss,"%1d",myIoTObj.sw1);
  doc["sw1"]  = String(ss);
  String buf;
  serializeJson(doc, buf);
  Serial.println(buf);
  serverHTTP.send(200, F("application/json"), buf); 
}

void sw1Event(void) {

  StaticJsonDocument<32> doc;
  DeserializationError error = deserializeJson(doc, serverHTTP.arg(0));
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;    
  }
  String sw1  = doc["sw1"];
  int sw1_len  = sw1.length() + 1;
  char buf[4];
  sprintf(buf,"%1d",myIoTObj.sw1);
  sw1.toCharArray(buf,sw1_len);
  myIoTObj.sw1 = sw1.toInt(); 

  if      (myIoTObj.sw1 == 0)   {  digitalWrite(RedLed, LOW);  }
  else                          { digitalWrite(RedLed, HIGH); }
  sprintf(buf,"%1d",myIoTObj.sw1);
  doc["sw1"] = String(buf);
  Serial.println(serverHTTP.arg(0));
  serverHTTP.send ( 200, "application/json", serverHTTP.arg(0));
}

void sw1GetEvent(void) {
  DynamicJsonDocument doc(32);
  char ss[4];
  sprintf(ss,"%1d",myIoTObj.sw1);
  doc["sw1"]  = String(ss);
  String buf;
  serializeJson(doc, buf);
  Serial.println(buf);
  serverHTTP.send(200, F("application/json"), buf);
}

void handlePage1(void) {
  serverHTTP.send( 200,"text/html", page2); 
}

void updateEvent(void) {
  StaticJsonDocument<128> doc;
  DeserializationError error = deserializeJson(doc, serverHTTP.arg(0));
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;    
  }
  String ssid = doc["ssid"];
  String pass = doc["pass"];
  String sw1  = doc["sw1"];
  int ssid_len = ssid.length() + 1;
  int pass_len = pass.length() + 1;
  int sw1_len  = sw1.length() + 1;
  ssid.toCharArray(myIoTObj.ssid,ssid_len);
  pass.toCharArray(myIoTObj.pass,pass_len);
  myIoTObj.tag = 0;
  writeObj(&myIoTObj);
  Serial.println(serverHTTP.arg(0));
  serverHTTP.send ( 200, "application/json", serverHTTP.arg(0));
}
void getEvent(void) {
  DynamicJsonDocument doc(64);
  doc["ssid"] = String(myIoTObj.ssid);
  doc["pass"] = String(myIoTObj.pass);
  String buf;
  serializeJson(doc, buf);
  Serial.println(buf);
  serverHTTP.send(200, F("application/json"), buf);
}


void handleRoot(void) { 
  serverHTTP.send( 200, "text/html", page1); 
}

void resetEvent(void) {
  StaticJsonDocument<32> doc;
  DeserializationError error = deserializeJson(doc, serverHTTP.arg(0));
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;    
  }
  ESP.restart();
}

void loop(void){
  serverHTTP.handleClient();  
 // msdn.update();
   if (intfired) {
    Serial.println("Interrupt Detected");
    intfired=false;
  }
}

void readObj(myIoT *obj) {
  byte *buf = (byte *)obj;
  EEPROM.begin(sizeof(myIoT));
  for (int i = 0; i < sizeof(myIoT); i++ ) { 
    buf[i] = EEPROM.read(i);
    delay(2);
  }
  EEPROM.end();
}

void writeObj(myIoT *obj) {
  byte *buf = (byte *)obj;
  EEPROM.begin(sizeof(myIoT));
  for (int i = 0; i < sizeof(myIoT) ; i++ ) {
    EEPROM.write(i, buf[i]);
    delay(10);
  }
  EEPROM.commit();
  EEPROM.end();
}
 
void chkUserData(myIoT *obj) {
  readObj(obj);
  Serial.println(obj->tag,HEX);
  Serial.println(obj->ssid);
  Serial.println(obj->pass);
  if((obj->tag) != 0 ) writeObj((myIoT *)&defaultObj);
  readObj(obj);
  Serial.printf("\ntag  : %x", obj->tag);
  Serial.printf("\nSSID : %s", obj->ssid);
  Serial.printf("\nPASS : %s", obj->pass);
  Serial.printf("\nSW1  : %d", obj->sw1 );
  Serial.println("");
}
