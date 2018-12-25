#pragma once

#include "Particle.h"

class Speaker;

class AudioSource {
    public:

    AudioSource();
    
    protected:

    friend class Speaker;
    
    Speaker *master;

    private:

};
