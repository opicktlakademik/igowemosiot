#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>

#include <iostream>
#include <string>


const char* apiKey = "igoWemosApiKey";
const char* ssid = "Onanimous";
const char* pass = "sayamengakuiopicktamvan";
ESP8266WebServer server(80);

void setIP(){
  startRequestToMyServer:
  HTTPClient httpclient;
  httpclient.begin("http://domainname.com/igoapi/setip"); //inisialisasi domain
  httpclient.addHeader("MyAPI", apiKey); // inisialisasi apikey on header
  //send request
  int httpCode = httpclient.GET();
  String payload = httpclient.getString();
  
  if(httpCode == 200 ){
    Serial.print("\n\n[RESPONSE FROM MY SERVER]\n");
    Serial.print("HTTP Code: ");
    Serial.println(httpCode);
    Serial.println(payload);
  }else{
    goto startRequestToMyServer;
  }
  httpclient.end();
  return;
}

void handleRoot(){
  if(server.hasArg("apikey") == false || server.arg("apikey") != apiKey){
    server.send(401, "text/plain", "Hi, you are not allow to access this wemos");
    return;
  }
  server.send(200, "text/plain", "Hi, you are here! i love you!");
}


void handleNotFound(){
  String message = "File Not Found \n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";  
  }
  server.send(404, "text/plain", message);
}

void checkCredential(){
  if(server.hasArg("ApiKey") == false || server.arg("ApiKey") != apiKey )
  {
    server.send(401, "text/plain", "API key is not correct for this wemos");
    return;
  }else if(server.hasArg("action")){
    server.send(401, "text/plain", "The data you sent is incorrect");
    return;
  }
}

void preparePin(){
  //init the pin
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  //set default value
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
  digitalWrite(D3, LOW);
  
}

void handleLight(){
  checkCredential();
  String message;
  String action = server.arg("action");

  if (action == "lamponeoff"){
    digitalWrite(D1, LOW);
    message = "Lampu satu sudah dimatikan";
  }else if(action == "lamponeoff"){
    digitalWrite(D1, HIGH);
    message = "Lampu satu sudah dihidupkan";
  }else if(action == "lamptwooff"){
    digitalWrite(D2, LOW);
    message = "Lampu dua sudah dimatikan";
  }else if(action == "lamptwoon"){
    digitalWrite(D2, HIGH);
    message = "Lampu dua sudah dihidupkan";
  }else if(action == "lampthreeoff"){
    digitalWrite(D3, LOW);
    message = "Lampu tiga sudah dimatikan";
  }else if(action == "lampthreeon"){
    digitalWrite(D3, HIGH);
    message = "Lampu tiga sudah dihidupkan";
  }else{
    message = "Aksi tidak diketahui";
    server.send(401, "text/plain", message);
    return;
  }
  server.send(200, "text/plain", message);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Welcome to Igo Light Controlling ");
  Serial.println(" ");
  Serial.printf("Connect to %s", ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  //wait for wifi connected
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println(" ");
  Serial.printf("\n [Connected] \n");
  Serial.printf("SSID: %s", ssid);
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println(" ");
  
  if(MDNS.begin("esp8266")){
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.on("/handleLight", handleLight);
  server.begin();
  setIP();
  delay(5000);
}
unsigned long long int last = 0;

void loop() {
    if(millis() > last + 300000){
      setIP();
      last = millis();
    }else{
      server.handleClient();
    }
}
