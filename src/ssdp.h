#pragma once

#include "Particle.h"

class Soundtouch;

class SSDPClient {
    public:

    SSDPClient(Soundtouch *soundtouch);

    void begin();

    void discover();

    private:

    Soundtouch *soundtouch;
    int broadcastPort;
    IPAddress broadcastIP;
    int listenPort;

};
