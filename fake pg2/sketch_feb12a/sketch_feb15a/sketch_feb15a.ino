#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>

const char* ssid = "Chat_Network";
const char* password = "";

AsyncWebServer server(80);
WebSocketsServer webSocket(81);

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_TEXT:
            for (int i = 0; i < webSocket.connectedClients(); i++) {
                webSocket.sendTXT(i, (char*)payload);
            }
            break;
    }
}

void setup() {
    Serial.begin(115200);
    WiFi.softAP(ssid, password);
    
    webSocket.begin();
    webSocket.onEvent(onWebSocketEvent);
    
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html", "<!DOCTYPE html>"
            "<html><head><title>ESP8266 Chat</title>"
            "<style>"
            "body { font-family: Arial, sans-serif; text-align: center; background: #f4f4f4; }"
            "#chatbox { width: 80%; height: 300px; border: 1px solid black; margin: auto; overflow-y: scroll; padding: 10px; background: white; text-align: left; }"
            "#inputArea { margin-top: 10px; }"
            "#msg { width: 60%; padding: 5px; }"
            "#sendBtn { padding: 5px; }"
            "</style></head><body>"
            
            "<h2>ESP8266 Chat</h2>"
            "<div id='chatbox'></div>"
            "<div id='inputArea'>"
            "<input id='msg' type='text' placeholder='Type a message...' onkeypress='handleKeyPress(event)'>"
            "<button id='sendBtn' onclick='sendMessage()'>Send</button>"
            "</div>"
            
            "<script>"
            "let ws = new WebSocket('ws://' + location.hostname + ':81/');"
            "let username = 'User_' + Math.floor(Math.random() * 9000 + 1000);"
            "ws.onmessage = event => {"
            "  let chatbox = document.getElementById('chatbox');"
            "  chatbox.innerHTML += event.data + '<br>';"
            "  chatbox.scrollTop = chatbox.scrollHeight;"
            "};"
            "function sendMessage() {"
            "  let msg = document.getElementById('msg').value.trim();"
            "  if (msg !== '') {"
            "    ws.send(username + ': ' + msg);"
            "    document.getElementById('msg').value = '';"
            "  }"
            "}"
            "function handleKeyPress(event) {"
            "  if (event.key === 'Enter') { sendMessage(); }"
           "} </script></body></html>");

    });
    
    server.begin();
}

void loop() {
    webSocket.loop();
}
