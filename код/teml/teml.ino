#include <ESP8266WiFi.h>
#include "DHT.h"
#define rel1 16
#define rel2 5
#define rel3 4
#define rel4 0
#define rel5 2
#define rel6 14
#define rel7 12
#define rel8 10
#define DHTTYPE DHT11
const char* ssid = "Ikar";
const char* password = "380980700507";
WiFiServer server(80);
const int DHTPin = 13;
DHT dht(DHTPin, DHTTYPE);
static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityTemp[7];
float t = 0;
String Temp;
float h = 0;
int maxt = 35;
int mint = 30;
String Maxt;
String Mint;
int i = 1;
String header;
void setup() {
  pinMode(16, OUTPUT);                                  // Встановлення відповідних портів в режим видачі сигналу
  pinMode(5, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(12, OUTPUT);
  digitalWrite (16, HIGH);                              // Встановлення відповідних портів в режим видачі логічної одиниці
  digitalWrite (5, HIGH);
  digitalWrite (4, HIGH);
  digitalWrite (0, HIGH);
  digitalWrite (2, HIGH);
  digitalWrite (14, HIGH);
  digitalWrite (12, HIGH);
  digitalWrite (10, HIGH);
  Serial.begin(115200);
  delay(10);
  dht.begin();
  Serial.println();
  Serial.print("Connecting to "); 
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  server.begin();
  Serial.println("Web server running. Waiting for the ESP IP...");
  delay(100);
  Serial.println(WiFi.localIP());
}
void loop() {
  t = dht.readTemperature();
  h = dht.readHumidity();
  Temp = t;
  Maxt = maxt;
  Mint = mint;
  if (t > maxt) {
    digitalWrite(rel1, LOW);
    digitalWrite(rel2, LOW);
  }
  if (t < mint) {
    digitalWrite(rel1, HIGH);
    digitalWrite(rel2, HIGH);
    digitalWrite(rel3, LOW);
    digitalWrite(rel4, LOW);
  }
  if (t > mint) {
    digitalWrite(rel3, HIGH);
    digitalWrite(rel4, HIGH);
  }
  if (mint >= maxt) {
    mint = mint - 1;
  }
  if (maxt <= mint) {
    maxt = maxt + 1;
  }

  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            
            if (header.indexOf("GET /Maxt/plus") >= 0) {
              maxt = maxt + 1;
            } else if (header.indexOf("GET /Maxt/minus") >= 0) {
              maxt = maxt - 1;
            } else if (header.indexOf("GET /Mint/plus") >= 0) {
              mint = mint + 1;
            } else if (header.indexOf("GET /Mint/minus") >= 0) {
              mint = mint - 1;
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" );
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");

            // Web Page Heading
            client.println("<body><h1>Регулятор теплиця</h1>");

            // Display current state, and ON/OFF buttons for GPIO 5
            client.println("<p>Поточна температура " + Temp + " C</p>");
            client.println("<p>Задана максимальна " + Maxt + " C</p>");
            client.println("<p>Задана мінімальна " + Mint + " C</p>");
            if (1) {
              client.println("<p><a href=\"/Maxt/plus\"><button class=\"button\">Maxt+</button></a></p>");
            }
            if (1) {
              client.println("<p><a href=\"/Maxt/minus\"><button class=\"button\">Maxt-</button></a></p>");
            }
            if (1) {
              client.println("<p><a href=\"/Mint/plus\"><button class=\"button\">Mint+</button></a></p>");
            }
            if (1) {
              client.println("<p><a href=\"/Mint/minus\"><button class=\"button\">Mint-</button></a></p>");
            }
            if (1) {
              client.println("<p><a href=\"/\"><button class=\"button\">Home</button></a></p>");
            }
            // If the output5State is off, it displays the ON button
            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
  delay(10);
}
