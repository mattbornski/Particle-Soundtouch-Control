#pragma once

#include "Particle.h"
#include "speaker.h"
#include "ssdp.h"

const int MAX_SPEAKERS = 8;

class SoundtouchClient {
    public:

    static void setDebug(int debug);
    static int getDebug();

    SoundtouchClient();
    void discover();

    // Ways to get Speaker instances:
    // Recommended: invoke repeatedly in your loop.  Returns a pointer to Speaker instance if found,
    // else returns NULL
    Speaker *discoverSpeakerWithCache(String friendlyName);
    
    // Not recommended, has potentially infinite delay loop:
    Speaker *blockingDiscoverSpeakerWithCache(String friendlyName);

    // Only viable after a call to discover().  Returns a pointer to Speaker instance if found,
    // else returns NULL
    Speaker *cachedSpeaker(String name);

    void clearCache();

    void messWith(String name);
    
    protected:

    static int debug;

    friend class SSDPClient;
    Speaker *addSpeaker(Speaker &speaker);

    private:

    SSDPClient *ssdpClient;

    uint32_t cachedIpAddresses[MAX_SPEAKERS];
    Speaker *knownSpeakers[MAX_SPEAKERS];

    void probe(String location);
    void probeCachedIpAddresses();
    void cacheKnownSpeakers();
};
