#pragma once

#include "Particle.h"

class Speaker {
    public:

    Speaker(String ipAddressString);

    bool probe();
    bool refresh();

    void play();
    void pause();

    void setVolume();
    void changeVolume(int delta);

    String deviceId;
    String friendlyName;

    private:

    String request(const char *method, const char *path, String body);
    String get(const char *path);
    String post(const char *path, String body);

    void key(const char *keyName);

    IPAddress ipAddress;
    int lastKnownVolume;
    bool lastKnownIsPlaying;

};
