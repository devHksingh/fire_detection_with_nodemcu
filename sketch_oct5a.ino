/*
  Fire Detection System using NodeMCU ESP8266
  Class 7 Project

  What this program does:
  1. Reads the flame sensor.
  2. If fire is detected -> buzzer ON + built-in LED ON.
  3. Shows SAFE / FIRE status on a web page (open it on phone or laptop).
  4. Prints messages in the Serial Monitor (baud rate 115200).
*/

#include <ESP8266WiFi.h>        // library to connect to Wi-Fi
#include <ESP8266WebServer.h>   // library to make a web server


// ---------- 1. WI-FI DETAILS (CHANGE THESE) ----------
const char* ssid     = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";


// ---------- 2. PIN NUMBERS ----------
const int flamePin  = D5;           // flame sensor DO pin
const int buzzerPin = D6;           // buzzer + pin
const int ledPin    = LED_BUILTIN;  // small LED on the NodeMCU board


// ---------- 3. VARIABLES ----------
ESP8266WebServer server(80);        // web server on port 80
bool lastFireState = false;         // remembers the previous state (to print only when it changes)


// ---------- 4. WEB PAGE (normal HTML + CSS) ----------
// Everything between R"rawliteral( and )rawliteral" is plain HTML/CSS,
// just like you write in web development.
// Words like %COLOR% and %MESSAGE% are placeholders.
// The NodeMCU replaces them with real values before sending the page.
const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta http-equiv="refresh" content="2">
  <title>Fire Detection System</title>

  <style>
    body {
      font-family: Arial, sans-serif;
      text-align: center;
      color: white;
      background-color: %COLOR%;
      padding: 40px 10px;
    }

    .box {
      background-color: rgba(0, 0, 0, 0.25);
      border-radius: 16px;
      padding: 20px;
      max-width: 400px;
      margin: auto;
    }

    .status {
      font-size: 56px;
      font-weight: bold;
      margin: 40px 0;
    }
  </style>
</head>

<body>
  <div class="box">
    <h1>Fire Detection System</h1>
    <div class="status">%STATUS%</div>
    <p>%MESSAGE%</p>
    <p>Made by: Harshit Atl instructor, Class 7</p>
  </div>
</body>
</html>
)rawliteral";


// ---------- 5. FUNCTION: CHECK FIRE ----------
// Returns true if fire is detected, false if safe.
// Most flame sensors give LOW when they see fire.
// If your sensor works the opposite way, change LOW to HIGH.
bool fireDetected() {
  return digitalRead(flamePin) == LOW;
}


// ---------- 6. FUNCTION: SEND WEB PAGE ----------
// Runs every time someone opens the page in a browser.
void handleRoot() {
  Serial.println("Web page requested by a browser.");

  String page = FPSTR(webpage);     // copy the HTML into a String

  if (fireDetected()) {
    page.replace("%COLOR%", "#d32f2f");     // red background
    page.replace("%STATUS%", "&#128293; FIRE!");
    page.replace("%MESSAGE%", "Flame detected. Stay safe and call for help!");
  } else {
    page.replace("%COLOR%", "#2e7d32");     // green background
    page.replace("%STATUS%", "&#9989; SAFE");
    page.replace("%MESSAGE%", "No fire detected.");
  }

  server.send(200, "text/html", page);      // send the page to the browser
}


// ---------- 7. SETUP (runs once at start) ----------
void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("=== Fire Detection System Started ===");

  // Set pin modes
  pinMode(flamePin, INPUT);       // sensor sends data IN
  pinMode(buzzerPin, OUTPUT);     // buzzer gets data OUT
  pinMode(ledPin, OUTPUT);        // LED gets data OUT

  // Start with buzzer OFF and LED OFF
  digitalWrite(buzzerPin, LOW);
  digitalWrite(ledPin, HIGH);     // built-in LED is OFF when HIGH (it works in reverse)
  Serial.println("Pins ready. Buzzer OFF, LED OFF.");

  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");            // prints dots while waiting
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");
  Serial.print("Open this address in your browser: http://");
  Serial.println(WiFi.localIP());

  // Start the web server
  server.on("/", handleRoot);     // when someone opens "/", run handleRoot()
  server.begin();
  Serial.println("Web server started.");
  Serial.println("Watching for fire...");
}


// ---------- 8. LOOP (runs again and again) ----------
void loop() {
  server.handleClient();          // listen for browser requests

  bool fire = fireDetected();     // read the sensor

  if (fire) {
    digitalWrite(buzzerPin, HIGH);   // buzzer ON
    digitalWrite(ledPin, LOW);       // built-in LED ON (LOW = ON)
  } else {
    digitalWrite(buzzerPin, LOW);    // buzzer OFF
    digitalWrite(ledPin, HIGH);      // built-in LED OFF (HIGH = OFF)
  }

  // Print a message only when the state changes
  if (fire != lastFireState) {
    if (fire) {
      Serial.println("WARNING: FIRE DETECTED! Buzzer ON, LED ON.");
    } else {
      Serial.println("Fire gone. System SAFE. Buzzer OFF, LED OFF.");
    }
    lastFireState = fire;         // remember the new state
  }
}
