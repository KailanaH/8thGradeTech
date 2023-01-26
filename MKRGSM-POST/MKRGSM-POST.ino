#include <MKRGSM.h>
#include <MkrGsm1400IoT.h>
#include <ArduinoJson.h>

/*

  Web client

  This sketch connects to a website through a MKR GSM 1400 board. Specifically,

  this example downloads the URL "http://www.example.org/" and

  prints it to the Serial monitor.

  Circuit:

   MKR GSM 1400 board

   Antenna

   SIM card with a data plan

  created 8 Mar 2012

  by Tom Igoe

*/

// libraries
#include <MKRGSM.h>

//#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number

const char PINNUMBER[]     = "";
// APN data

const char GPRS_APN[]      = "hologram"; // sim card identification 

const char GPRS_LOGIN[]    = "";

const char GPRS_PASSWORD[] = "";

// initialize the library instance

GSMClient client;

GPRS gprs;

GSM gsmAccess(false);// true to turn on modem debugging

DynamicJsonDocument doc(1024);

// URL, path and port (for example: www.example.com)
char server[] = "www.example.com";
char path[] = "/";
int port = 80; // port 80 is the default for HTTP

double latitude = 20.2;

double longitude = 121.3;

double altitude = 14.0;

void setup() {

  // initialize serial communications and wait for port to open:

  Serial.begin(9600);

  while (!Serial) {

    ; // wait for serial port to connect. Needed for native USB port only

  }

  Serial.println("Starting Arduino web client.");

  // connection state

  bool connected = false;

  // After starting the modem with GSM.begin()

  // attach the shield to the GPRS network with the APN, login and password

  while (!connected) {

    if ((gsmAccess.begin(PINNUMBER) == GSM_READY) &&

        (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)) {

      connected = true;
      Serial.println("KH connected");

    } else {

      Serial.println("Not connected");

      delay(1000);

    }

  }

  Serial.println("checking battery --");
  Serial.println(analogRead(ADC_BATTERY));

  while (analogRead(ADC_BATTERY) * (4.3 / 1023) < 3.6) {
    Serial.println("battery level too low to start cellular traffic");
    delay(20000);
  }

  Serial.println("connecting");

  if (client.connect(server, port)) {

    Serial.println("connected");

    // Make a HTTP request:

    client.print("GET ");

    client.print(path);

    client.println(" HTTP/1.1");

    client.print("Host: ");

    client.println(server);

    client.println("Connection: close");

    client.println();

  } else {

    // if you didn't get a connection to the server:

    Serial.println("connection failed");

  }
}

//void loop() {
//
//  // if there are incoming bytes available
//
//  // from the server, read them and print them:
//
//  if (client.available()) {
//
//   // char c = client.read();
//
////    Serial.print(c);
//
//  }
//
//  // if the server's disconnected, stop the client:
//
//  if (!client.available() && !client.connected()) {
//
//    Serial.println();
//
//    Serial.println("disconnecting.");
//
//    client.stop();
//
//    // do nothing forevermore:
//
//    for (;;)
//
//      ;
//
//  }
//}

void loop() {

 // ...GPS stuff happens here

  // print GPS values

  Serial.println();

  Serial.print("Location: ");

  Serial.print(latitude, 7);

  Serial.print(", ");

  Serial.println(longitude, 7);

  Serial.print("Altitude: ");

  Serial.print(altitude);

  Serial.println("m");

  // Serial.print("Number of satellites: ");

 //  Serial.println(satellites);

  doc["latitude"] = latitude;
  doc["longitude"] = longitude;
  doc["altitude"] = altitude;
  
  String location;
  serializeJson(doc, location);
  Serial.print("Location string: ");
  Serial.println(location);

  Serial.println();
  
  Serial.println("Starting Arduino web client.");

  // connection state
  
  if (client.available()) {
    Serial.println("Sending location");
    client.print("POST ");
    client.print(path);
    client.println(" HTTP/1.1");
    delay(5);
    client.println("Content-Type: application/json;");
    delay(5);
    client.print("Content-Length: ");
    client.println(location.length());
    client.print("Host: ");
    client.println(server);
    delay(5);
    client.println("Connection: close");
    delay(5);
    client.println();
    delay(5);
    client.println(location);
    Serial.print("Successfully sent location");

    if (!client.available() && !client.connected()) {
  
      Serial.println();
  
      Serial.println("disconnecting.");
  
      client.stop();
    }
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  
}
