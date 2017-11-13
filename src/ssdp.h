#pragma once

#include "Particle.h"

class SoundtouchClient;

class SSDPClient {
    public:

    SSDPClient(SoundtouchClient *soundtouchClient);

    void discover();

    private:

    SoundtouchClient *soundtouchClient;
    int broadcastPort;
    IPAddress broadcastIP;
    int listenPort;

};
