#pragma once

#include "Particle.h"

class SoundtouchClient;

class SSDPClient {
public:

    SSDPClient(SoundtouchClient *soundtouchClient);
    ~SSDPClient();

    void discover();
    void cleanupDiscovery();

protected:
    void dealloc();
    void report(String speakerAddress);

private:

    SoundtouchClient *soundtouchClient;

    UDP *broadcastUdpClient;
    uint32_t lastSsdpPacketTx;
};
