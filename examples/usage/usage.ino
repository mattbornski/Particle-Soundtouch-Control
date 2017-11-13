// Example usage for soundtouch library by mattbornski.

#include "soundtouch.h"

// One client can be used to manage multiple speakers
SoundtouchClient soundtouchClient;

Speaker *office;
Speaker *library;

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

    if (office == NULL) {
        office = soundtouchClient.discoverWithCache("Office");
    }
    if (library == NULL) {
        library = soundtouchClient.discoverWithCache("Library");
    }

    if (office != NULL && library != NULL && office->online && library->online) {
        if (office->playing && !library->playing) {
            library->setSource(office);
        } else if (library->playing && !office->playing) {
            office->setSource(library);
        }
    }

    msDelay = millis();
}
