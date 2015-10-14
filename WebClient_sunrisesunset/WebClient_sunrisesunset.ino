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
#include <EthernetV2_0.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(10,10,10,198); // local IP

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;
#define W5200_CS  10
#define SDCARD_CS 4

char sunriseSunsetServer[] = "http://api.sunrise-sunset.org";

/**
 * Method to fetch sunrise and sunset from server
 */
void getSunriseAndSunset() {
  if(client.connect(sunriseSunsetServer, 80)) {
    Serial.println("connected to sunriseSunset server");
    // Make a HTTP request:
    client.println("GET /json?lat=40.547554&lng=-89.614399&date=today HTTP/1.1");
//    client.println("Host: api.sunrise-sunset.org");
//    client.println("Connection: keep-alive");
//    client.println("Accept: text/html");
//    client.println("User-Agent: Arduino Mega2560");
    client.println();
  } else {
    Serial.println("Connection failed");
  }
  delay(2000);
}

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // disable w5100 SPI while starting SD
  pinMode(W5200_CS,OUTPUT);
  digitalWrite(W5200_CS,HIGH);
  
//  pinMode(SDCARD_CS,OUTPUT);
//  digitalWrite(SDCARD_CS,HIGH);//Deselect the SD card
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
  delay(1000);
  Serial.println("ok.");
  Serial.println("connecting...");

  // Ethernet begin returns with its SPI enabled, so disable it.
  digitalWrite(W5200_CS,HIGH);

  Serial.print("My address is ");
  Serial.println(Ethernet.localIP());

  getSunriseAndSunset();

//  // if you get a connection, report back via serial:
//  if (client.connect(sunriseSunsetServer, 80)) {
//    Serial.println("connected");
//    // Make a HTTP request:
//    client.println("GET /search?q=arduino HTTP/1.0");
//    client.println();
//  } 
//  else {
//    // kf you didn't get a connection to the server:
//    Serial.println("connection failed");
//  }
}

void loop()
{
  // if there are incoming bytes available 
  // from the server, read them and print them:
  if (client.available()) {
    Serial.println("client available");
    char c = client.read();
    Serial.print(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    for(;;)
      ;
  }
}

