/*
  Web client
 
 This sketch connects to a website (http://www.google.com)
 using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 
 created 18 Dec 2009
 modified 9 Apr 2012
 by David A. Mellis
 
 */

#include <SPI.h>
#include <SD.h>
// ethernet library from https://github.com/Seeed-Studio/Ethernet_Shield_W5200
#include <EthernetV2_0.h>
// httpclient library from https://github.com/amcewen/HttpClient
#include <HttpClient.h>

// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30*1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(10,10,10,198); // local IP

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;
HttpClient http(client);
#define W5200_CS  10
#define SDCARD_CS 4

// parameters needed to fetch sunrise and sunset time RESTfully.
const char sunriseSunsetServer[] = "api.sunrise-sunset.org";
const char sunriseSunsetPath[] = "/json?lat=40.547554&lng=-89.614399&date=today";
IPAddress sunriseSunsetIP(104,131,2,15);


/**
 * Method to fetch sunrise and sunset time
 */
void getSunriseSunsetHTTP() {
  int err = 0;
  err = http.get(sunriseSunsetIP, sunriseSunsetServer, sunriseSunsetPath);
  if(err == 0) {
    Serial.println("connection ok");
    err = http.responseStatusCode();

    if(err >= 0) {
      Serial.print("response status code: ");
      Serial.println(err);

      // TODO: should check here that response code is 200, which means "OK"
      err = http.skipResponseHeaders();
      if(err >= 0) {
        int bodyLen = http.contentLength();
        Serial.print("content length: ");
        Serial.println(bodyLen);
        Serial.println("Body returned:");

        unsigned int timeoutStart = millis();
        char c;
        // while we're connected to the host and we've not yet hit the timeout, process response.
        while( (http.connected() || http.available()) && ((millis() - timeoutStart) < kNetworkTimeout)) {
          if(http.available()) {
            c = http.read();
            Serial.print(c);

            bodyLen--;
            // We read something, reset the timeout counter
            timeoutStart = millis();
            
          } else {
              // We haven't got any data, so let's pause to allow some to
              // arrive
              delay(kNetworkDelay);
          }
        }
      } else {
        Serial.print("Failed to skip response headers: ");
        Serial.println(err);
      }
    } else {
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  } else {
    Serial.print("Connection failed: ");
    Serial.println(err);
  }
  
  Serial.println("disconnecting.");
  http.stop();
}

/**
 * Setup method
 */
void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // disable w5100 SPI while starting SD
  pinMode(W5200_CS,OUTPUT);
  digitalWrite(W5200_CS,HIGH);
  
  Serial.println("Starting SD...");
  if(!SD.begin(SDCARD_CS)) {
    Serial.println("failed.");
  } else {
    Serial.println("ok.");
  }
  
  // start the Ethernet connection:
  Serial.println("Starting Ethernet...");
  Ethernet.begin(mac, ip);

  // give time for ethernet shield to initialize.
  delay(4000);
  Serial.println("ok.");
  Serial.println("connecting...");

  // Ethernet begin returns with its SPI enabled, so disable it.
  digitalWrite(W5200_CS,HIGH);

  Serial.print("My address is ");
  Serial.println(Ethernet.localIP());
}

void loop()
{
  getSunriseSunsetHTTP();

    // do nothing forevermore:
    while(true);
}

