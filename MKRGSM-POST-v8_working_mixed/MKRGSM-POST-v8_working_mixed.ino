#include <ArduinoHttpClient.h>
#include <MKRGSM.h>
#include <TinyGPS++.h>
#include "wiring_private.h"
#include <Arduino.h>



// look at how many decimals and stuff other things that idk
void displayInfo();

// Cellular variables
const char PINNUMBER[]     = "";
const char GPRS_APN[]      = "hologram";
const char GPRS_LOGIN[]    = "";
const char GPRS_PASSWORD[] = "";


// GPS variables
static const uint32_t GPSBaud = 9600;


// GSM objects
GSMSSLClient client;
GPRS gprs;
GSM gsmAccess;

// GPS object
TinyGPSPlus gps;

char server[] = "kailana-tracker.herokuapp.com";
char path[] = "/api/v1/checkins/";
int port = 443;


HttpClient httpClient = HttpClient(client, server, port);
String response;
int statusCode = 0;
String dataStr;


void setup() {

  Serial.begin(9600);
  Serial1.begin(GPSBaud);
  while (!Serial) {
    Serial.println("Booting");
  }

  Serial.println("Starting GSM connection to Wia!.");
  boolean connected = false;

  while (!connected) {
    if ((gsmAccess.begin(PINNUMBER) == GSM_READY) &&
        (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)) {
      connected = true;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("connecting...");
  if (client.connect(server, port)) {
    Serial.println("connected");
  } else {
    Serial.println("connection failed");//if no server connection
  }
}

void loop ()
{

  // read from GPS
  // Set variable we want to post

  while (Serial1.available() > 0)
    if (gps.encode(Serial1.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while (true);
  }

  Serial.print(F("Location: "));
  if (gps.location.isValid())
  {

    if (client.connect(server, port)) {

      float voltage = analogRead (ADC_BATTERY) * (4.3/1023.0) ;
      postToWia("{\"latitude\":" + String(gps.location.lat()) + ", \"longitude\":" + String(gps.location.lng(),6) + ", \"deviceID\":\"doggie\", \"accuracy\": 21,\"battery\":" + String(voltage) + "}");

      statusCode = httpClient.responseStatusCode();
      response = httpClient.responseBody();

      //Serial.print("Status code: ");
      //Serial.println(statusCode);
      //Serial.print("Response: ");
      //Serial.println(response);

    } else {
      Serial.println("connection failed");
    }


  }
  else
  {
    Serial.print(F("INVALID"));
  }
  Serial.println();


  delay(10000); // Wait for 10 seconds to post again --------is it only posting once? right????????????


}


void postToWia(String data) {

  Serial.println(server);
  Serial.println(port);
  Serial.println(path);
  Serial.println(data);

  httpClient.beginRequest();
  httpClient.post(path); // this is /api/v1/checkins
  httpClient.sendHeader("Authorization", "Token fcf8b1ddde0682ab879bd7e8fd5360821c156f7a");
  httpClient.sendHeader("Content-Type", "application/json");
  httpClient.sendHeader("Content-Length", data.length());
  httpClient.endRequest();
  httpClient.print(data);

}

void displayInfo()
{
  Serial.print(F("Location: "));
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }
  Serial.println();
}
