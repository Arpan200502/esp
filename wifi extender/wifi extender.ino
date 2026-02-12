#include <WiFi.h>
#include <esp_wifi.h>
#include <esp32_nat_router.h>  // ✅ Include the NAT Router Library

// Credentials for Wi-Fi A (Internet Source)
const char *wifi_A_SSID = "Arpan";  // Change this
const char *wifi_A_PASS = "9836870854";  // Change this

// Credentials for Wi-Fi B (ESP32 Hotspot)
const char *wifi_B_SSID = "ESP32_Hotspot"; 
const char *wifi_B_PASS = "";  

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n[+] Initializing Wi-Fi...");

    // Set ESP32 to AP + STA mode
    WiFi.mode(WIFI_AP_STA);

    // Connect to Wi-Fi A
    Serial.print("[+] Connecting to Wi-Fi A: ");
    Serial.println(wifi_A_SSID);
    WiFi.begin(wifi_A_SSID, wifi_A_PASS);

    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED && attempt < 20) {
        Serial.print(".");
        delay(500);
        attempt++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n[+] Wi-Fi A Connected!");
        Serial.print("[+] IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\n[!] Failed to connect to Wi-Fi A.");
        return;
    }

    // Start Wi-Fi B (Access Point Mode)
    Serial.println("[+] Starting Wi-Fi B (Hotspot)...");
    WiFi.softAP(wifi_B_SSID, wifi_B_PASS);
    
    Serial.print("[+] Wi-Fi B IP Address: ");
    Serial.println(WiFi.softAPIP());

    // ✅ Enable NAT (Network Address Translation)
    Serial.println("[+] Enabling NAT...");
    enableNAT(WiFi.localIP(), WiFi.softAPIP());  

    Serial.println("[+] NAT & Routing Enabled!");
}

void loop() {
    delay(10000);
}
