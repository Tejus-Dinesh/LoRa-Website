
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <LoRa.h>
#define SS 15
#define RST 16
#define DIO0 2
#define host "Yashash"
#define pass "9449802010"
WiFiServer server(80);
WiFiClient client;
String rule;
String data = "00:00:00:00:00:00";
bool isReq = false;

void CheckNewReq(){
  client = server.available();
  if (!client) {
    return;
  }
  while (client.connected() && !client.available()) {
    delay(1);
  }

  String req = client.readStringUntil('\r');
  int addr_start = req.indexOf(' ');
  int addr_end = req.indexOf(' ', addr_start + 1);
  if (addr_start == -1 || addr_end == -1) {
    Serial.print("Invalid request: ");
    Serial.println(req);
    return;
  }
  req = req.substring(addr_start + 1, addr_end);
  Serial.print("Requested Path: ");
  Serial.println(req);

  rule = req;
  isReq = true;
  client.flush();
}


void waitUntilNewReq(){
  do {CheckNewReq();} while (!isReq);
  isReq = false;
}


void publishData(String final_data){
  String s;
  //HTTP Protocol code.
  s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  s += final_data; //Our final raw data to return
  client.print(s);
  
}




void setup(){
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(host,pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(host);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
// Setting up mDNS responder
  if (!MDNS.begin("esp8266")) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
// Start TCP (HTTP) server
  server.begin();
  Serial.println("TCP server started");
// Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
  Serial.println("Receiver Host");
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa Error");
    while (1);
}
}
void loop(){
  
  
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
  waitUntilNewReq();
    Serial.print("Receiving Data: ");
    while (LoRa.available()) {
     data = LoRa.readString();
      Serial.println(data);
      publishData(data);
     
  }
  } 
}
