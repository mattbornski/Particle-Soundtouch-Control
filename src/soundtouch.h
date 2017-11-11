#pragma once

#include "Particle.h"
#include "ssdp.h"

const int MAX_SPEAKERS = 8;

class Speaker;

class Soundtouch {
    public:

    Soundtouch();

    void begin();

    void discover();

    void probe(String location);

    int indexOfSpeakerWithDeviceId(String deviceId);

    Speaker *speakerWithName(String name);

    Speaker *anySpeaker();

    void addSpeaker(Speaker *speaker);

    void messWith(String name);
    
    private:

    SSDPClient *ssdpClient;

    Speaker *speakers[MAX_SPEAKERS];

};
