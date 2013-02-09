/*
 Facebook Like Box
 
 This sketch connects to Facebook using an Ethernet shield. It parses the XML
 returned, and looks for <fan_count>this is a tweet</fan_count>
 
 You can use the Arduino Ethernet shield, or the Adafruit Ethernet shield, 
 either one will work, as long as it's got a Wiznet Ethernet module on board.
 
 This example uses the DHCP routines in the Ethernet library which is part of the 
 Arduino core from version 1.0 beta 1
 
 This example uses the String library, which is part of the Arduino core from
 version 0019.  

 This example uses a Display Shield to display de results of the call to facebook.
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * LCD Display shield attached to pins 8, 9, 4, 5, 6, 7
 
 created in 8 Aug 2012
 by David Alcubierre
 
 Ethernet part based on 'Twitter client with strings' by Tom Igoe (public domain).
 
 This code is in the public domain.
 
 */
#include <SPI.h>
#include <Ethernet.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd( 8, 9, 4, 5, 6, 7 );

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x01 };
IPAddress ip(192,168,1,20);

// initialize the library instance:
EthernetClient client;

const unsigned long requestInterval = 12000;  // delay between requests

char serverName[] = "api-read.facebook.com";  // facebook URL

boolean requested;                   // whether you've made a request since connecting
unsigned long lastAttemptTime = 0;            // last time you connected to the server, in milliseconds

String currentLine = "";            // string to hold the text from server
String fbcount = "";                  // string to hold the tweet
boolean readingFbcount = false;       // if you're currently reading the tweet

void setup() {
  // reserve space for the strings:
  currentLine.reserve(256);
  fbcount.reserve(100);

 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  // attempt a DHCP connection:
  Serial.println("Attempting to get an IP address using DHCP:");
  if (!Ethernet.begin(mac)) {
    // if DHCP fails, start with a hard-coded address:
    Serial.println("failed to get an IP address using DHCP, trying manually");
    Ethernet.begin(mac, ip);
  }
  Serial.print("My address:");
  Serial.println(Ethernet.localIP());
  // connect to Twitter:
  connectToServer();

}

void loop()
{
  if (client.connected()) {
    if (client.available()) {
      // read incoming bytes:
      char inChar = client.read();

      // add incoming byte to end of line:
      currentLine += inChar; 

      // if you get a newline, clear the line:
      if (inChar == '\n') {
        currentLine = "";
      } 
      // if the current line ends with <fan_count>, it will
      // be followed by the tweet:
      if ( currentLine.endsWith("<fan_count>")) {
        // tweet is beginning. Clear the tweet string:
        readingFbcount = true; 
        fbcount = "";
      }
      // if you're currently reading the bytes of the facebook count,
      // add them to the tweet String:
      if (readingFbcount) {
        if (inChar != '<') {
          fbcount += inChar;
        } 
        else {
          // if you got a "<" character,
          // you've reached the end of the facebo:
          readingFbcount = false;
          Serial.println(fbcount); 
          lcd.begin(16, 2);
          lcd.print("Boing Boing likes");
          lcd.setCursor(0,7);
          lcd.print(fbcount);
  
          // close the connection to the server:
          client.stop(); 
        }
      }
    }   
  }
  else if (millis() - lastAttemptTime > requestInterval) {
    // if you're not connected, and two minutes have passed since
    // your last connection, then attempt to connect again:
    connectToServer();
  }
}

void connectToServer() {
  // attempt to connect, and wait a millisecond:
  Serial.println("connecting to server...");
  String content = "";
  
  if (client.connect(serverName, 80)) {
    Serial.println("making HTTP request...");
    // make HTTP GET request to Facebook:
    client.println("GET /restserver.php?format=xml&method=fql.multiquery&pretty=0&queries={%22page_info%22%3A%22select%20name%2Cfan_count%2Cpage_url%2Ctype%20from%20page%20where%20page_id%20IN%20(27479046178)%22}&sdk=joey HTTP/1.1");
    // declare correct server
    client.print("HOST: " + content.concat(serverName));
    client.println();
  }
  // note the time of this connect attempt:
  lastAttemptTime = millis();
}
