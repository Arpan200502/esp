#include "FS.h"
#include "SD.h"
#include "SPI.h"

void setup() {
    Serial.begin(115200);
    if (!SD.begin(12)) {  // Change '5' if using a different CS pin
        Serial.println("Card Mount Failed");
        return;
    }
    Serial.println("SD Card Initialized");
}

void loop() {
}
