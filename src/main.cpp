#include <Arduino.h>

#include <WiFiClientSecure.h>
// #include <ArduinoJson.h>
#include <knownwifi.h>

// #include <toneAC.h>

void print_mac(const unsigned char *mac);
void macTest();

// Melody liberated from the toneMelody Arduino example sketch by Tom Igoe.
int melody[] = {262, 196, 196, 220, 196, 0, 247, 262};
int noteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4};

/* WIFI ZONE */
const char *host = "esp32";
int status = WL_IDLE_STATUS; // the Wifi radio's status
unsigned long wifiLastCheck = 0;
long checkForWifiInterval = 30000; // milliseconds between checks for wifi
void joinWifi();
/* END WIFI ZONE */

const char *server = "lambcam.webstaging.co.uk"; // Server URL

// Port 443 is the default for HTTPS
const int httpsPort = 443;

// Root cert for ISRG Root X1 - for the render server. Works on LocalTunnel but not on ngrok.
const char *test_root_ca = "-----BEGIN CERTIFICATE-----\n" \
"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\n" \
"-----END CERTIFICATE-----\n";

WiFiClientSecure client;

unsigned int frequency = 1500;
unsigned int duration = 200;

void done()
{
  Serial.println("Done!");
}

String readString;
String c;

char * thing;

#define RXD2 25
#define TXD2 26
const unsigned int MAX_MESSAGE_LENGTH = 30;
int baudrate = 9600;
// StaticJsonDocument<96> doc;
char output[96];

String wifiMacString;
void readRFID();
void postRFID();


void setup()
{
  
  // doc["sensor"] = "handeid";
  // doc["eid"] = 123;

  Serial.begin(115200);
  delay(100);

  // Wifi connection
  joinWifi();

  // set the id to the mac address
  String wifiMacString = WiFi.macAddress();
  wifiMacString.replace(":", "");
  Serial.println(wifiMacString);

  // Initialise the RFID reader on Serial2
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2, false, 50);

  Serial.println("setup complete");

 
  // thing = "VER\r";
  // Serial2.write(thing, sizeof(thing));
  // thing = "MOS\r";
  // Serial2.write(thing, sizeof(thing));
}

void loop()
{

  if (Serial.available())
  {
    c = Serial.readString(); // read the incoming data as string

   // Serial2.write(c, sizeof(c));
  }

  readRFID();
  
}


void readRFID()
{
  // if there's data available from the rfid reader, read a packet
  if (Serial2.available())
  {
    // Create a place to hold the incoming message
    static char message[MAX_MESSAGE_LENGTH];
    static unsigned int message_pos = 0;

    // Read the next available byte in the serial receive buffer
    char inByte = Serial2.read();

    // Message coming in (check not terminating character) and guard for over message size
    if (inByte != '\r' && (message_pos < MAX_MESSAGE_LENGTH - 1))
    {
      // Add the incoming byte to our message
      message[message_pos] = inByte;
      message_pos++;
    }
    // Full message received...
    else
    {
      // Add null character to string
      message[message_pos] = '\0';

      // Print the message (or do other things)
      Serial.println(message);
      // doc["eid"] = message;
      // serializeJson(doc, output);
      // Serial.println(output);
      // doc["sensor"] = "handeid";
      // doc["user"] = 1;
      String queryString = "sensor=" + wifiMacString + "&eid = " + String(message);
      // "{\"api_key\":\"tPmAT5Ab3j7F9\",\"sensor\":\"BME280\",\"value1\":\"24.25\",\"value2\":\"49.54\",\"value3\":\"1005.14\"}");
      
      postRFID(queryString);

      // Reset for the next message
      message_pos = 0;
    }
  }
}

void postRFID(String queryString)
{
  Serial.println("\nStarting connection to server...");
  client.setCACert(test_root_ca);
  if (!client.connect(server, 443))
  {
    Serial.println("Connection failed!");
  }
  else
  {
    client.println("POST /api/v1/device/read-tag HTTP/1.1");
    client.println("Host: lambcam.webstaging.co.uk");
    client.println("User-Agent: Arduino/1.0");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(queryString.length());
    client.println("Connection: close");
    client.println();
    client.println(queryString);
    client.println();
    while (client.connected())
    {
      String line = client.readStringUntil('\n');
      if (line == "\r")
      {
        Serial.println("headers received");
        break;
      }
    }
    // if there are incoming bytes available
    // from the server, read them and print them:
    while (client.available())
    {
      char c = client.read();
      Serial.write(c);
    }

    client.stop();
  }
}

void joinWifi()
{

  Serial.println("scan start");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();

  for (int i = 0; i < n; ++i)
  {
    // Serial.println(WiFi.SSID(i));
    // Print SSID and RSSI for each network found
    for (int j = 0; j < noKnown; ++j)
    {   
        // Serial.print(">>>");
        // Serial.println(knownWifi[j][0]);
        if (WiFi.SSID(i) == knownWifi[j][0])
        {

          Serial.print("Connecting to ");

          Serial.println(knownWifi[j][0]);

          WiFi.mode(WIFI_STA);
          WiFi.begin(knownWifi[j][0], knownWifi[j][1]);

          while (WiFi.status() != WL_CONNECTED)
          {
            delay(500);
            Serial.print(".");
          }
          Serial.println("");
          Serial.println("WiFi connected");

          // Start the server
          // server.begin();
          // Serial.println("Server started");

          // Print the IP address
          Serial.print("Use this URL : ");
          Serial.print("http://");
          Serial.print(WiFi.localIP());
          Serial.println("/");

          // timeClient.update();
          // Serial.println(timeClient.getFormattedTime());

          return;
        }
    }
  }
}