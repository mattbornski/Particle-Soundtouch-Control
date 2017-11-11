// Example usage for soundtouch library by mattbornski.

#include "soundtouch.h"
#include "speaker.h"

// One client can be used to manage multiple speakers
Soundtouch soundtouch;

void setup() {
    // Initialize client
    soundtouch.begin();
}

uint32_t msDelay;
void loop() {
    if ((millis() - msDelay) < 10000) {
        // Early return from loop gives control back to the Particle framework.
        // This is going to be nicer for everybody than idle waiting.
        return;
    }

    
    
    Speaker *speaker = soundtouch.speakerWithName("Family Room");
    if (speaker != NULL) {
        Serial.println("I know about " + speaker->friendlyName);
        speaker->refresh();
        soundtouch.messWith(speaker->friendlyName);
    } else {
        // Discover speakers
        soundtouch.discover();
    }

    msDelay = millis();
}
