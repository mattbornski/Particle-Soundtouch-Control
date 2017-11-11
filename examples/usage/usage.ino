// Example usage for soundtouch library by mattbornski.

#include "soundtouch.h"
#include "speaker.h"

// One client can be used to manage multiple speakers
Soundtouch soundtouch;

// Caching our speaker allows us to 
Speaker *cachedSpeaker;

void setup() {
    Serial.println("+ Setup");
    delay(1000);
}

uint32_t msDelay;
void loop() {
    if ((millis() - msDelay) < 10000) {
        // Early return from loop gives control back to the Particle framework.
        // This is going to be nicer for everybody than idle waiting.
        return;
    }

    if (cachedSpeaker == NULL) {
        cachedSpeaker = soundtouch.discoverWithCache("Family Room");
    }

    if (cachedSpeaker != NULL) {
        soundtouch.messWith(cachedSpeaker->friendlyName);
    }

    msDelay = millis();
}
