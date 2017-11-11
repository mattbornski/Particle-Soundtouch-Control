#pragma once

#include "Particle.h"

class Speaker {
    public:

    Speaker(String ipAddressString);
    Speaker(uint32_t ipAddressNumber);

    void play();
    void pause();

    void setVolume(int level);
    void changeVolume(int delta);

    bool validated;
    String deviceId;
    String friendlyName;
    IPAddress ipAddress;

    private:

    void initialize(IPAddress ipAddress);

    String request(const char *method, const char *path, String body);
    String get(const char *path);
    String post(const char *path, String body);

    bool key(const char *keyName);

    bool internalSetVolume(int level);

    bool probe();
    bool refreshMedia();
    bool refreshVolume();

    int lastKnownVolume;
    bool lastKnownIsPlaying;
    bool lastKnownIsActive;

};
