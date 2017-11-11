#pragma once

#include "Particle.h"

class Speaker {
    public:

    Speaker(String ipAddressString);
    Speaker(uint32_t ipAddressNumber);

    bool probe();
    bool refresh();

    void play();
    void pause();

    void setVolume();
    void changeVolume(int delta);

    String deviceId;
    String friendlyName;
    IPAddress ipAddress;

    private:

    String request(const char *method, const char *path, String body);
    String get(const char *path);
    String post(const char *path, String body);

    bool key(const char *keyName);

    int lastKnownVolume;
    bool lastKnownIsPlaying;
    bool lastKnownIsActive;

};
