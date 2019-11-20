#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <RCSwitch.h>

// int status = WL_IDLE_STATUS;     // the Wifi radio's status
WiFiClient client;
const char* ssid     = "wifi-login";
const char* password = "wifi-password";

int idx;

RCSwitch mySwitch = RCSwitch();

ESP8266WebServer server(80);

 const char * domoticz_server = "192.168.179.248"; //Domoticz port
 int port = 8080; //Domoticz port

// Network information
// IP must match the IP in config.py in python folder
IPAddress ip(10, 11, 0, 1);
// Set gateway to your router's gateway
IPAddress gateway(10, 11, 0, 254);
IPAddress subnet(255, 255, 255, 0);



int getArgValue(String name)
{
  for (uint8_t i = 0; i < server.args(); i++)
    if(server.argName(i) == name)
      return server.arg(i).toInt();
  return -1;
}

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
    mySwitch.enableTransmit(12);
    
    WiFi.mode(WIFI_STA);
    WiFi.hostname("ESP-002");
    WiFi.config(ip, gateway, subnet);
    WiFi.begin(ssid, password);
    Serial.println("");
    // Connect to wifi and print the IP address over serial
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());


 server.on("/", handleRoot);

/*
This is an automation (but still manual) configuration to turn off/on the lights.
the lighX_X_on_domo is made for main control (from domoticz), lightX_X_on is made to reset the state in domoticz to on or off
every new light should have the 4 different commands, and the functions made and bound to them.
 */
server.on("/reset", handleResetArgs); //Associate the handler function to the path
server.on("/switch", handleSwitchArgs); //Associate the handler function to the path

 server.begin();
  Serial.println("HTTP server started");

}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  
}

void handleRoot() {
  String message = "<html><head></head><body style='font-family: sans-serif; font-size: 12px'>Please read the manual,<br><br>";
  message += "have fun -<a href='http://youtube.com/bitlunislab'>bitluni</a></body></html>";
  server.send(200, "text/html", message);
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}


void handleResetArgs() { //Handler

String message = "Number of args received:";
message += server.args();            //Get number of parameters
message += "\n";                            //Add a new line

for (int i = 0; i < server.args(); i++) {

message += "Arg number" + (String)i + " –> ";   //Include the current iteration value
message += server.argName(i) + ": ";     //Get the name of the parameter
message += server.arg(i) + "\n";             //Get the value of the parameter

String argumentName = (server.argName(i));
String argumentValue = (server.arg(i));
//String (server.argName(i)) = (server.arg(i));
if (argumentName == "idxX") {
  char* idxX = "";
  idxX = (char*) argumentValue.c_str();
  if (client.connect(domoticz_server,port)) {client.print("GET /json.htm?&type=command&param=switchlight&idx=");
        client.print(idxX);client.print("&switchcmd=Off");
        client.println(" HTTP/1.1");client.print("Host: ");client.print(domoticz_server);client.print(":");client.println(port);client.println("User-Agent: Arduino-ethernet");client.println("Connection: close");client.println();
        client.stop();}
} 
}
server.send(200, "text/plain", message);       //Response to the HTTP request
}

void handleSwitchArgs() { //Handler

String message = "Number of args received:";
message += server.args();            //Get number of parameters
message += "\n";                            //Add a new line

for (int i = 0; i < server.args(); i++) {

message += "Arg number" + (String)i + " –> ";   //Include the current iteration value
message += server.argName(i) + ": ";     //Get the name of the parameter
message += server.arg(i) + "\n";             //Get the value of the parameter

String argumentName = (server.argName(i));
String argumentValue = (server.arg(i));

//String (server.argName(i)) = (server.arg(i));
if (argumentName == "dataX") {
  char* dataX = "";
  dataX = (char*) argumentValue.c_str();
  mySwitch.sendTriState(dataX);delay(100);
}

}
server.send(200, "text/plain", message);       //Response to the HTTP request
}
