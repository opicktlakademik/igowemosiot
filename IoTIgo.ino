#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>


const char* apiKey = "igoWemosApiKey";
const char* ssid = "Onanimous";
const char* pass = "sayamengakuiopicktamvan";
ESP8266WebServer server(80);



setIP(){
  startRequestToMyServer:
  HTTPClient httpclient;
  httpclient.begin('http://domainname.com/igoapi/setip'); //inisialisasi domain
  httpclient.addHeader("MyAPI", apiKey); // inisialisasi apikey on header
  //send request
  int httpCode = http.GET();
  String payload = httpclient.getString();
  
  if(httpCode == 200 ){
  Serial.print("\n\n[RESPONSE FROM MY SERVER]\n");
  Serial.print("HTTP Code: ");
  Serial.println(httpCode);
  Serial.println(payload);
  }else{
    goto startRequestToMyServer;
  }
  
}

void handleRoot(){
  if(!server.hasArg('apikey') || server.arg('apikey') != apiKey){
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
  if(!server.hasArg('ApiKey') || server.arg('ApiKey') != apiKey )
  {
    server.send(401, "text/plain", "API key is not correct for this wemos");
    return;
  }else if(!server.hasArg('action') || !server.hasArg('lamp') || server.arg('lamp') < 1 || server.arg('lamp') > 3){
    server.send(401, "text/plain", "The data you sent is invalid");
    return;
  }
}

void preparePin(){
  //init the pin
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  //set default value
  digitalWrite(1, LOW);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  
}

void handleLight(){
  checkCredential();
  String action = " ";
  String message = " ";
  int lamp;

  action = server.arg('action');
  lamp = server.arg('lamp');

  switch(action){
    case 'off' :
      //action
      digitalWrite(lamp, LOW);
      message = "Lampu " + lamp + " berhasil di nyalakan";
      break;
    case 'on' :
      //action
      digitalWrite(lamp, HIGH);
      message = "Lampu " + lamp + " berhasil di matikan";
      break;
    default :
      //action
      message = "Tidak ada aksi yang di tentukan";
      break;
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
  mcAsClient();
  delay(5000);
}

void loop() {
    mcAsClient();
    delay(5000);
}
