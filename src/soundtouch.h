#pragma once

#include "Particle.h"
#include "ssdp.h"

const int MAX_SPEAKERS = 8;

class Speaker;

class Soundtouch {
    public:

    Soundtouch();
    void discover();

    // Ways to get Speaker instances:
    Speaker *loadFromCache();
    Speaker *discoverWithCache(String friendlyName);

    // Only viable after a call to discover();
    Speaker *knownSpeakerWithName(String name);
    Speaker *anyKnownSpeaker();

    // For use with loadFromCache (automatically invoked by discoverWithCache)
    void setCached(Speaker *speaker);
    void clearCache();

    void messWith(String name);
    
    protected:

    friend class SSDPClient;
    void addSpeaker(Speaker *speaker);

    private:

    SSDPClient *ssdpClient;

    Speaker *speakers[MAX_SPEAKERS];

    void probe(String location);

    int indexOfSpeakerWithDeviceId(String deviceId);
};
