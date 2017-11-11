#include "soundtouch.h"
#include "speaker.h"
#include "ssdp.h"

Soundtouch::Soundtouch() {
    ssdpClient = new SSDPClient(this);
}

void Soundtouch::messWith(String name) {
    Serial.println("+ Mess around");
    Speaker *speaker = this->knownSpeakerWithName(name);
    if (speaker != NULL) {
        speaker->play();
        delay(5000);
        speaker->changeVolume(10);
        delay(5000);
        speaker->changeVolume(-20);
        delay(5000);
        speaker->changeVolume(10);
        delay(5000);
        speaker->pause();
    }
}

Speaker *Soundtouch::loadFromCache() {
    // Attempt to lookup IP address of this speaker from cache
    uint32_t ipAddressNumber;
    EEPROM.get(0, ipAddressNumber);

    // 0 is cleared EEPROM
    // 4294967295 is max uint32_t which is uninitialized EEPROM
    if (ipAddressNumber == 0 || ipAddressNumber == 4294967295) {
        return NULL;
    }
    // Confirm speaker is still answering at this address
    Serial.println("  - Checking cached IP address");
    Speaker *probedSpeaker = new Speaker(ipAddressNumber);
    if (probedSpeaker->validated) {
        this->addSpeaker(probedSpeaker);
        return probedSpeaker;
    }
    delete probedSpeaker;
    return NULL;
}

Speaker *Soundtouch::discoverWithCache(String friendlyName) {
    Speaker *speaker = this->knownSpeakerWithName(friendlyName);
    
    if (speaker == NULL) {
        // Not already known in RAM, go to EEPROM
        Speaker *cachedSpeaker = this->loadFromCache();
        if (cachedSpeaker->friendlyName.compareTo(friendlyName) == 0) {
            return cachedSpeaker;
        }
        delete cachedSpeaker;
    }

    // Not in RAM or EEPROM, go to network
    discover();

    // Now loaded in RAM if it exists
    speaker = this->knownSpeakerWithName(friendlyName);
    if (speaker != NULL) {
        // Persist in EEPROM as well
        this->setCached(speaker);
    }
    return speaker;
}

// Currently I only allow caching a single speaker instance
void Soundtouch::setCached(Speaker *speaker) {
    uint32_t ipAddressNumber =
          (uint32_t)speaker->ipAddress[0] * 256*256*256
        + (uint32_t)speaker->ipAddress[1] * 256*256
        + (uint32_t)speaker->ipAddress[2] * 256
        + (uint32_t)speaker->ipAddress[3];
    EEPROM.put(0, ipAddressNumber);
}

void Soundtouch::clearCache() {
    EEPROM.put(0, (uint32_t)0);
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

Speaker *Soundtouch::knownSpeakerWithName(String name) {
    for (int index = 0; index < MAX_SPEAKERS; index++) {
        Speaker *speaker = this->speakers[index];
        if (speaker != NULL) {
            if (name.length() == 0 || speaker->friendlyName.compareTo(name) == 0) {
                return speaker;
            }
        }
    }
    return NULL;
}

Speaker *Soundtouch::anyKnownSpeaker() {
    return this->knownSpeakerWithName("");
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
