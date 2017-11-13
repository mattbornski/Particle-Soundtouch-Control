#pragma once

#include "Particle.h"

// class ContentSource;

class SoundtouchClient;

class Speaker {
    public:

    Speaker(String ipAddressString);
    Speaker(uint32_t ipAddressNumber);

    bool probe();

    void play();
    void pause();

    void setVolume(int level);
    void changeVolume(int delta);

    void setSource(Speaker *master);

    bool online;
    bool playing;
    // ContentSource *source;
    String deviceId;
    String friendlyName;
    IPAddress ipAddress;

    protected:

    friend class SoundtouchClient;
    void copy(Speaker *speaker);

    private:

    void initialize(IPAddress ipAddress);

    String request(const char *method, const char *path, String body);
    String get(const char *path);
    String post(const char *path, String body);

    bool key(const char *keyName);

    bool internalSetVolume(int level);
    // bool internalSetSource(ContentSource *source);

    bool refreshMedia();
    bool refreshVolume();

    int lastKnownVolume;
    bool lastKnownIsActive;

};
