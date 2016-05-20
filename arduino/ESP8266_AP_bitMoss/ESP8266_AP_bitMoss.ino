#include <ESP8266WiFi.h>
#include "DHT.h"

#define DHTTYPE DHT11
#define DHTPIN  2

const int ANALOG_PIN = A0; 

WiFiServer server(80);
DHT dht(DHTPIN, DHTTYPE, 11); // 11 works fine for ESP826
float humidity, temp;  // Values read from sensor

unsigned long previousMillis = 0;        // will store last temp was read
const long interval = 2000;              // interval at which to read sensor

void setup() 
{
  initHardware();
  pinMode(DHTPIN, INPUT);
  setupWiFi();
  server.begin();
}

void loop() 
{
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  int val = -1; // We'll use 'val' to keep track of both the
                // request type (read/set) and value if set.
  if (req.indexOf("/read") != -1)
    val = -2; // Will print pin reads
 
  client.flush();

  // Prepare the response. Start with the common header:
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";
  if (val == -2)
  { // If we're reading pins, print out those values:
    //s += "Moisture Value  = ";
    s += String(analogRead(ANALOG_PIN)/1024 * 100);
    s += ","; // Go to the next line.

    if(millis() - previousMillis >= interval) 
    {
      previousMillis = millis();
      temp = dht.readTemperature();
      humidity = dht.readHumidity();
    }
    s += String((int)temp)+","+String((int)humidity);
  }
  else
  {
    s += "Invalid Request.<br>Try /read.";
  }
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}

void setupWiFi()
{
  WiFi.mode(WIFI_AP);

  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "ESP8266 TinyFarmer " + macID;

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i=0; i<AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar);
}

void initHardware()
{
  Serial.begin(115200);
  dht.begin();  
  
}




