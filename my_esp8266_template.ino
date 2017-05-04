#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <functional>
#include "switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"


#pragma region Globals


//------------------------------------------------------------------
const char * ssid = "Xtreme";
const char * password = "winfastgeforce";
String device_name = "my switch";
String sHostName = "ESP-01";
//------------------------------------------------------------------

uint8_t connection_state = 0; // Connected to WIFI or not
uint16_t reconnect_interval = 10000; // If not connected wait time to try again
String myIPAdd = "";

#pragma endregion Globals

UpnpBroadcastResponder upnpBroadcastResponder;
Switch * device = NULL;


String ipToString(IPAddress ip) {
  String s = "";
  for (int i = 0; i < 4; i++)
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  return s;
} //- See more at: http://www.esp8266.com/viewtopic.php?p=25466#sthash.XtvKG3FD.dpuf






uint8_t WiFiConnect(const char * nSSID = nullptr, const char * nPassword = nullptr) {

  static uint16_t attempt = 0;

  Serial.print("Connecting to ");

  if (nSSID) {
    WiFi.begin(nSSID, nPassword);
    Serial.println(nSSID);
  }
  else  {
    WiFi.begin(ssid, password);
    Serial.println(ssid);
  }

  uint8_t i = 0;

  while (WiFi.status() != WL_CONNECTED && i++ < 50) {
    delay(200);
    Serial.print(".");
    digitalWrite(2, LOW);
    delay(200);
    digitalWrite(2, HIGH);
  }

  ++attempt;

  Serial.println("");

  if (i == 51) {

    Serial.print("Connection: TIMEOUT on attempt: ");
    Serial.println(attempt);
    if (attempt % 2 == 0)
      Serial.println("Check if access point available or SSID and Password\r\n");

    digitalWrite(2, LOW);
    return false;

  }

  Serial.println("Connection: ESTABLISHED");
  Serial.print("Got IP address: ");

  myIPAdd = ipToString(WiFi.localIP());

  Serial.println(WiFi.localIP());
  digitalWrite(2, HIGH);

  return true;
}



void Awaits() {
  uint32_t ts = millis();
  while (!connection_state) {
    delay(50);
    if (millis() > (ts + reconnect_interval) && !connection_state) {
      connection_state = WiFiConnect();
      ts = millis();
    }
  }
}





void deviceOn() {

}

void deviceOff() {

}




void addDevice() {

  //Serial.println("begin udp multicast");
  upnpBroadcastResponder.beginUdpMulticast();

  // Define your switches here. Max 14
  // Format: Alexa invocation name, local port no, on callback, off callback
  device = new Switch(device_name, 80, deviceOn, deviceOff);

  upnpBroadcastResponder.addDevice( * device);

}








void setup() {

  Serial.begin(9600);

  WiFi.hostname(sHostName);
  
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);

  // Initialise wifi connection
  connection_state = WiFiConnect();
  if (!connection_state) // if not connected to WIFI
    Awaits(); // constantly trying to connect


  if (connection_state) {
    addDevice();
  }


}




void loop() {

  upnpBroadcastResponder.serverLoop();
  device-> serverLoop();

}
