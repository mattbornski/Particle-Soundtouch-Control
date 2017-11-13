#include "soundtouch.h"
// #include "contentsource.h"
#include "speaker.h"
#include "ssdp.h"

SoundtouchClient::SoundtouchClient() {
    ssdpClient = new SSDPClient(this);
    for (int index = 0; index < MAX_SPEAKERS; index++) {
        this->cachedIpAddresses[index] = 0;
        this->knownSpeakers[index] = NULL;
    }
}

void SoundtouchClient::probeCachedIpAddresses() {
    // Load cached IP addresses
    Speaker *possibleSpeakers[MAX_SPEAKERS];
    for (int index = 0; index < MAX_SPEAKERS; index++) {
        EEPROM.get(index * sizeof(uint32_t), this->cachedIpAddresses[index]);
        // 0 is cleared EEPROM
        // 4294967295 is max uint32_t which is uninitialized EEPROM
        if (this->cachedIpAddresses[index] != 0 && this->cachedIpAddresses[index] != 4294967295) {
            possibleSpeakers[index] = new Speaker(this->cachedIpAddresses[index]);
        } else {
            possibleSpeakers[index] = NULL;
        }
    }
    for (int possibleSpeakerIndex = 0; possibleSpeakerIndex < MAX_SPEAKERS; possibleSpeakerIndex++) {
        Speaker *possibleSpeaker = possibleSpeakers[possibleSpeakerIndex];
        if (possibleSpeaker == NULL) {
            continue;
        }
        bool foundMatch = false;
        for (int knownSpeakerIndex = 0; knownSpeakerIndex < MAX_SPEAKERS; knownSpeakerIndex++) {
            Speaker *knownSpeaker = this->knownSpeakers[knownSpeakerIndex];
            if (knownSpeaker != NULL && knownSpeaker->ipAddress == IPAddress(possibleSpeaker->ipAddress)) {
                foundMatch = true;
                if (!knownSpeaker->online) {
                    // A speaker that we cached could easily be offline when we first checked,
                    // but is worth probing again.
                    knownSpeaker->probe();
                }
                break;
            }
        }
        if (!foundMatch) {
            this->addSpeaker(possibleSpeaker);
        } else {
            delete possibleSpeaker;
        }
    }
}

void SoundtouchClient::cacheKnownSpeakers() {
    for (int index = 0; index < MAX_SPEAKERS; index++) {
        uint32_t speakerIpAddressNumber = 0;
        Speaker *speaker = this->knownSpeakers[index];
        if (speaker != NULL) {
            speakerIpAddressNumber = 
                (uint32_t)speaker->ipAddress[0] * 256*256*256
              + (uint32_t)speaker->ipAddress[1] * 256*256
              + (uint32_t)speaker->ipAddress[2] * 256
              + (uint32_t)speaker->ipAddress[3];
        }
        if (speakerIpAddressNumber != this->cachedIpAddresses[index]) {
            this->cachedIpAddresses[index] = speakerIpAddressNumber;
            EEPROM.put(index * sizeof(uint32_t), speakerIpAddressNumber);
        }
    }
}

void SoundtouchClient::clearCache() {
    for (int index = 0; index < MAX_SPEAKERS; index++) {
        this->cachedIpAddresses[index] = 0;
        EEPROM.put(index * sizeof(uint32_t), 0);
    }
}

void SoundtouchClient::messWith(String name) {
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

Speaker *SoundtouchClient::discoverWithCache(String friendlyName) {
    Speaker *speaker = this->knownSpeakerWithName(friendlyName);
    if (speaker != NULL) {
        return speaker;
    }
    
    // Not already known in RAM, go to EEPROM
    probeCachedIpAddresses();
    // Now loaded in RAM if we remembered it's IP address
    speaker = this->knownSpeakerWithName(friendlyName);
    if (speaker != NULL) {
        return speaker;
    }

    // Not in RAM or EEPROM, go to network
    discover();
    // Now loaded in RAM if it is answering on the network
    speaker = this->knownSpeakerWithName(friendlyName);
    if (speaker != NULL) {
        // Cache it so we can find it faster next boot
        cacheKnownSpeakers();
    }
    return speaker;
}

void SoundtouchClient::discover() {
    Serial.println("+ Discovery");
    ssdpClient->discover();
}

Speaker *SoundtouchClient::knownSpeakerWithName(String name) {
    // Search for an online speaker with the right name
    for (int index = 0; index < MAX_SPEAKERS; index++) {
        Speaker *speaker = this->knownSpeakers[index];
        if (speaker != NULL && speaker->online) {
            if (name.length() == 0 || speaker->friendlyName.compareTo(name) == 0) {
                return speaker;
            }
        }
    }
    return NULL;
}

Speaker *SoundtouchClient::addSpeaker(Speaker *newSpeaker) {
    // Supersedes any other speakers with the new speaker's device ID or IP address
    // If no empty slot is available, overwrite an offline speaker.
    int firstOfflineSpeakerIndex = -1;
    int firstEmptySpeakerIndex = -1;
    int matchedIndex = -1;
    for (int index = 0; index < MAX_SPEAKERS; index++) {
        if (this->knownSpeakers[index] == NULL) {
            if (firstEmptySpeakerIndex == -1) {
                firstEmptySpeakerIndex = index;
            }
        } else {
            Speaker *oldSpeaker = this->knownSpeakers[index];
            bool matched = 
                   (oldSpeaker->deviceId.compareTo(newSpeaker->deviceId))
                || (oldSpeaker->ipAddress == newSpeaker->ipAddress);
            if (matched) {
                if (matchedIndex != -1) {
                    // Already matched something
                    delete this->knownSpeakers[index];
                    oldSpeaker = NULL;
                    if (firstEmptySpeakerIndex == -1) {
                        firstEmptySpeakerIndex = index;
                    }
                } else {
                    // This is our first match, we'll keep it
                    matchedIndex = index;
                    // Make sure it matches all of our information
                    oldSpeaker->copy(newSpeaker);
                }
            } else {
                if (firstOfflineSpeakerIndex == -1 && !oldSpeaker->online) {
                    firstOfflineSpeakerIndex = index;
                }
            }
        }
    }

    if (matchedIndex != -1) {
        // We matched an existing speaker, delete the new one.
        delete newSpeaker;
        return this->knownSpeakers[matchedIndex];
    } else {
        // We did not match an existing speaker, add this one.
        if (firstEmptySpeakerIndex != -1) {
            this->knownSpeakers[firstEmptySpeakerIndex] = newSpeaker;
        } else if (firstOfflineSpeakerIndex != -1) {
            delete this->knownSpeakers[firstOfflineSpeakerIndex];
            this->knownSpeakers[firstOfflineSpeakerIndex] = newSpeaker;
        } else {
            // Well you're screwed now.
            // TODO LRU ?
            int randomIndex = rand() % MAX_SPEAKERS;
            delete this->knownSpeakers[randomIndex];
            this->knownSpeakers[randomIndex] = newSpeaker;
        }
        return newSpeaker;
    }
}
