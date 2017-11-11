#include "soundtouch.h"
#include "speaker.h"
#include "ssdp.h"

Soundtouch::Soundtouch() {
    ssdpClient = new SSDPClient(this);
}

void Soundtouch::begin() {
    ssdpClient->begin();
}

void Soundtouch::messWith(String name) {
    Speaker *speaker = this->speakerWithName(name);
    if (speaker != NULL) {
        speaker->play();
        delay(5000);
        speaker->pause();
        delay(5000);
        speaker->play();
        delay(5000);
        speaker->pause();
    }
}

void Soundtouch::discover() {
    Serial.println("+ Discovery");
    ssdpClient->discover();
}

int Soundtouch::indexOfSpeakerWithDeviceId(String deviceId) {
    for (int index = 0; index < MAX_SPEAKERS; index++) {
        Speaker *speaker = this->speakers[index];
        if (speaker == NULL) {
            if (deviceId.compareTo("") == 0) {
                return index;
            }
        } else if (speaker->deviceId.compareTo(deviceId) == 0) {
            return index;
        }
    }
    return -1;
}

Speaker *Soundtouch::speakerWithName(String name) {
    for (int index = 0; index < MAX_SPEAKERS; index++) {
        Speaker *speaker = this->speakers[index];
        Serial.println(String::format("Inspecting speaker at index %d", index));
        if (speaker != NULL) {
            Serial.println("  -> " + speaker->deviceId + " '" + speaker->friendlyName + "'");
            Serial.println("  ? '" + name + "' = '" + speaker->friendlyName + "'");
            Serial.println(speaker->friendlyName.compareTo(name) == 0);
            if (name.length() == 0 || speaker->friendlyName.compareTo(name) == 0) {
                return speaker;
            }
        }
    }
    return NULL;
}

Speaker *Soundtouch::anySpeaker() {
    return this->speakerWithName("");
}



void Soundtouch::addSpeaker(Speaker *speaker) {
    int index = this->indexOfSpeakerWithDeviceId(speaker->deviceId);
    if (index != -1) {
        delete this->speakers[index];
    } else {
        index = this->indexOfSpeakerWithDeviceId("");
    }
    this->speakers[index] = speaker;
}
