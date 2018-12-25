#pragma once

#define MAX_SPEAKERS 16

#include "Particle.h"
#include "speaker.h"

class AudioSource {
    public:

    AudioSource();

    bool join(Speaker *);
    
    protected:

    friend class Speaker;
    
    Speaker *masterSpeaker;
    Speaker *memberSpeakers[];

    private:

};
