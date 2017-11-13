#pragma once

#include "Particle.h"
#include "speaker.h"
#include "ssdp.h"

const int MAX_SPEAKERS = 8;

class SoundtouchClient {
    public:

    SoundtouchClient();
    void discover();

    // Ways to get Speaker instances:
    Speaker *discoverWithCache(String friendlyName);

    // Only viable after a call to discover();
    Speaker *knownSpeakerWithName(String name);

    void clearCache();

    void messWith(String name);
    
    protected:

    friend class SSDPClient;
    Speaker *addSpeaker(Speaker *speaker);

    private:

    SSDPClient *ssdpClient;

    uint32_t cachedIpAddresses[MAX_SPEAKERS];
    Speaker *knownSpeakers[MAX_SPEAKERS];

    void probe(String location);
    void probeCachedIpAddresses();
    void cacheKnownSpeakers();
};
