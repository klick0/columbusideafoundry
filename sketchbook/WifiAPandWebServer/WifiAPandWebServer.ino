//11/16/2022 Initial code for Idea Foundry Chandelier

#include <WiFi.h>

#define LEDPIN 2
hw_timer_t *mytimer = NULL;
uint64_t count = 0;

// Replace with your network credentials
const char *ssid = "ChandyAP";
const char *password = "123456789";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

bool rotationenabled=false;
bool rotationdirection=true;
String rotationspeed="low";

const int speedlow = 100;
const int speedmed = 300;
const int speedhigh = 600;

#define STEPPIN 4
#define DIRPIN 5

void IRAM_ATTR onTimer() {
    count++;

    //if (count%1000) {
        //Serial.print("count: ");
        //Serial.println(count);
    //}

    //if (count%2==0) {
    //    digitalWrite(LEDPIN, HIGH);
    //} else {
    //    digitalWrite(LEDPIN, LOW);
    //}

    if (rotationenabled) {
        digitalWrite(DIRPIN, (rotationdirection?1:0));
        digitalWrite(STEPPIN, (count%2));
    }
    
}

void setupTimerAlarm() {
    if (rotationspeed=="low") {
        timerAlarmWrite(mytimer, 100000/speedlow, true);
    } else if (rotationspeed=="med") {
        timerAlarmWrite(mytimer, 100000/speedmed, true);
    } else if (rotationspeed=="high") {
        timerAlarmWrite(mytimer, 100000/speedhigh, true);
    }
}

void setup()
{
    Serial.begin(115200);
	// Initialize the output variables as outputs
    pinMode(LEDPIN, OUTPUT);
	// Set outputs to LOW
    digitalWrite(LEDPIN, HIGH);

    pinMode(STEPPIN, OUTPUT);
    pinMode(DIRPIN, OUTPUT);

    //setup timer
    mytimer = timerBegin(0, 80, true);
    timerAttachInterrupt(mytimer, &onTimer, true);
    setupTimerAlarm();
    //timerAlarmWrite(mytimer, 1000, true);
    timerAlarmEnable(mytimer);

	// Connect to Wi-Fi network with SSID and password
	Serial.print("Setting AP (Access Point)…");
	// Remove the password parameter, if you want the AP (Access Point) to be open
	WiFi.softAP(ssid, password);

	IPAddress IP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(IP);

	server.begin();
}


void loop()
{
	WiFiClient client = server.available();	// Listen for incoming clients

	if (client) {		// If a new client connects,
		Serial.print("count: ");
        Serial.println(count);
		Serial.println("New Client.");	// print a message out in the serial port
		String currentLine = "";	// make a String to hold incoming data from the client
		while (client.connected()) {	// loop while the client's connected
			if (client.available()) {	// if there's bytes to read from the client,
				char c = client.read();	// read a byte, then
				Serial.write(c);	// print it out the serial monitor
				header += c;
				if (c == '\n') {	// if the byte is a newline character
					// if the current line is blank, you got two newline characters in a
					// row. that's the end of the client HTTP request, so send a response:
					if (currentLine.length() == 0) {
						// HTTP headers always start with a response code (e.g. HTTP/1.1 200
						// OK) and a content-type so the client knows what's coming, then a
						// blank line:
						client.println("HTTP/1.1 200 OK");
						client.println("Content-type:text/html");
						client.println("Connection: close");
						client.println();

                        if (header.indexOf("GET /rotation/on") >= 0) {
                            Serial.println("Rotation set to ON");
                            rotationenabled=true;                     
                        } else if (header.indexOf("GET /rotation/off") >= 0) {
                            Serial.println("Rotation set to OFF");
                            rotationenabled=false;
                            digitalWrite(DIRPIN, 0);
                            digitalWrite(STEPPIN, 0);
                        } else if (header.indexOf("GET /rotation/low") >= 0) {
                            Serial.println("Rotation Speed set to LOW");
                            rotationspeed="low";
                            setupTimerAlarm();
                        } else if (header.indexOf("GET /rotation/med") >= 0) {
                            Serial.println("Rotation Speed set to MED");
                            rotationspeed="med";
                            setupTimerAlarm();
                        } else if (header.indexOf("GET /rotation/high") >= 0) {
                            Serial.println("Rotation Speed set to HIGH");
                            rotationspeed="high";
                            setupTimerAlarm();
                        } else if (header.indexOf("GET /rotation/cw") >= 0) {
                            Serial.println("Rotation Direction set to 1");
                            rotationdirection=1;
                        } else if (header.indexOf("GET /rotation/ccw") >= 0) {
                            Serial.println("Rotation Direction set to 0");
                            rotationdirection=0;
                        }
                       

                                  
						// Display the HTML web page
						client.println("<!DOCTYPE html><html>");
						client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
						client.println("<link rel=\"icon\" href=\"data:,\">");
						// CSS to style the on/off buttons
						// Feel free to change the background-color and font-size attributes
						// to fit your preferences
						client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
						client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
						client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
						client.println(".button2 {background-color: #555555;}</style></head>");

						// Web Page Heading
						client.println("<body><h1>Idea Foundry Magical Chandelier</h1>");

                        client.println("<p>Enabled</p>");
                        if (rotationenabled) {
                            client.println("<p><a href=\"/rotation/on\"><button class=\"button\">ON</button></a>");
                            client.println("<a href=\"/rotation/off\"><button class=\"button button2\">OFF</button></a></p>"); 
                        } else {
                            client.println("<p><a href=\"/rotation/on\"><button class=\"button button2\">ON</button></a>");
                            client.println("<a href=\"/rotation/off\"><button class=\"button\">OFF</button></a></p>");
                        }

                        client.println("<p>Rotation Speed</p>");
                        if (rotationspeed=="low") {
                            client.println("<p><a href=\"/rotation/low\"><button class=\"button\">Low</button></a></p>");
                        } else {
                            client.println("<p><a href=\"/rotation/low\"><button class=\"button button2\">Low</button></a></p>");
                        }

                        if (rotationspeed=="med") {
                            client.println("<p><a href=\"/rotation/med\"><button class=\"button\">Medium</button></a></p>");
                        } else {
                            client.println("<p><a href=\"/rotation/med\"><button class=\"button button2\">Medium</button></a></p>");
                        }

                        if (rotationspeed=="high") {
                            client.println("<p><a href=\"/rotation/high\"><button class=\"button\">High</button></a></p>");
                        } else {
                            client.println("<p><a href=\"/rotation/high\"><button class=\"button button2\">High</button></a></p>");
                        }

                        client.println("<p>Direction</p>");
                        if (rotationdirection) {
                            client.println("<p><a href=\"/rotation/cw\"><button class=\"button\">CW</button></a>");
                            client.println("<a href=\"/rotation/ccw\"><button class=\"button button2\">CCW</button></a></p>");
                        } else {
                            client.println("<p><a href=\"/rotation/cw\"><button class=\"button button2\">CW</button></a>");
                            client.println("<a href=\"/rotation/ccw\"><button class=\"button\">CCW</button></a></p>");
                        }
           
						client.println("</body></html>");

						// The HTTP response ends with another blank line
						client.println();
						// Break out of the while loop
						break;
					} else {	// if you got a newline, then clear currentLine
						currentLine = "";
					}
				} else if (c != '\r') {	// if you got anything else but a carriage
					// return character,
					currentLine += c;	// add it to the end of the currentLine
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
}
