//
//  FlowerSystem.h
//  HelloCpp
//
//  Created by Zeyang Li on 8/15/13.
//
//

#pragma once

#include "ContentLayer.h"

namespace PH
{
    class FlowerDrop : public ContentLayer
    {
    private:
        FlowerDrop() : m_emitter(NULL) {}
        virtual ~FlowerDrop();
        
    public:
        bool init();
        void start();
        void stop();
        
        CREATE_FUNC(FlowerDrop);
        
    private:
        CCParticleSystemQuad *m_emitter;
    };
}