// MKRGSM - Version: 1.5.0
#include <MKRGSM.h>
#include <ArduinoJson.h>
#include <Arduino_MKRGPS.h>

const char PINNUMBER[]     = "";
// APN data

const char GPRS_APN[]      = SECRET_GPRS_APN;

const char GPRS_LOGIN[]    = "";

const char GPRS_PASSWORD[] = "";

DynamicJsonDocument doc(1024);

// initialize the library instance

GSMSSLClient client;

GPRS gprs;

GSM gsmAccess(true);

// Specify IP address or hostname

char server[] = "webhook.site";
char path[] = "/ce480c43-95a8-4f1e-b996-bc02f6d1adaa";
int port = 443; 

void setup() {

  // initialize serial communications and wait for port to open:

  Serial.begin(9600);

  while (!Serial) {

    ; // wait for serial port to connect. Needed for native USB port only

  }

  // If you are using the MKR GPS as shield, change the next line to pass

  // the GPS_MODE_SHIELD parameter to the GPS.begin(...)
  delay(200);


  if (!GPS.begin()) {

    Serial.println("Failed to initialize GPS!");

    while (1);

  }
  Serial.println("Starting Arduino GSM ping.");
  // connection state
  bool connected = false;

  // After starting the modem with GSM.begin()
  // attach the shield to the GPRS network with the APN, login and password
  while (!connected)
  {
    if ((gsmAccess.begin(PINNUMBER) == GSM_READY) &&
        (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY))
    {
      connected = true;
    } 
    else 
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  // Use this to load Root Certs the first time
  if (client.connect(server, port)) 
  // Use this to not load Root Certs anymore as done last time and stored in modem's flash
//  if (client.connect(server, port, false)) 
  {
        Serial.println("connected");

    // Make a HTTP request:

    client.print("GET ");

    client.print(path);

    client.println(" HTTP/1.1");

    client.print("Host: ");

    client.println(server);

    client.println("Connection: close");

    client.println();
  } 
  else 
  {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void loop() {

  ...GPS stuff happens here

  // print GPS values

  Serial.println();

  Serial.print("Location: ");

  Serial.print(latitude, 7);

  Serial.print(", ");

  Serial.println(longitude, 7);

  Serial.print("Altitude: ");

  Serial.print(altitude);

  Serial.println("m");

  Serial.print("Number of satellites: ");

  Serial.println(satellites);

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
