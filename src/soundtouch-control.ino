// Example usage for soundtouch library by mattbornski.

#include "soundtouch.h"

// One client can be used to manage multiple speakers
SoundtouchClient soundtouchClient;

Speaker *office;
Speaker *library;

void setup() {
    Serial.println("+ Setup");
    delay(1000);

    SoundtouchClient::setDebug(2);
}

uint32_t msDelay;
void loop() {
    if ((millis() - msDelay) < 10000) {
        // Early return from loop gives control back to the Particle framework.
        // This is going to be nicer for everybody than idle waiting.
        return;
    }

    if (office == NULL) {
        office = soundtouchClient.discoverSpeakerWithCache("Office");
    } else {
        Serial.println("Have office speaker");
    }
    if (library == NULL) {
        library = soundtouchClient.discoverSpeakerWithCache("Library");
    } else {
        Serial.println("Have library speaker");
    }

    if (office != NULL && library != NULL && office->online && library->online) {
        if (office->playing && !library->playing) {
            Serial.println("slaving library to office");
            library->setSource(office);
        } else if (library->playing && !office->playing) {
            Serial.println("slaving office to library");
            office->setSource(library);
        }
    }

    msDelay = millis();
}
