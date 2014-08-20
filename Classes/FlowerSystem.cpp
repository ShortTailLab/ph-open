//
//  FlowerSystem.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 8/15/13.
//
//

#include "FlowerSystem.h"

namespace PH
{
    bool FlowerDrop::init()
    {
        if(!ContentLayer::init())
            return false;
        
        m_emitter = CCParticleSystemQuad::createWithTotalParticles(6);
        m_emitter->retain();
        
        this->addChild(m_emitter, 10);
        m_emitter->setTexture( CCTextureCache::sharedTextureCache()->addImage("gameui/huaban_1.png") );
        
        // duration
        m_emitter->setDuration(-1);
        
        // gravity
        m_emitter->setEmitterMode(kCCParticleModeGravity);
        m_emitter->setGravity(ccp(0,-1));
        
        // angle
        m_emitter->setAngle(280);
        m_emitter->setAngleVar(20);
        
        // speed of particles
        m_emitter->setSpeed(120);
        m_emitter->setSpeedVar(20);
        
        // radial
        m_emitter->setRadialAccel(0);
        m_emitter->setRadialAccelVar(1);
        
        // tagential
        m_emitter->setTangentialAccel(30);
        m_emitter->setTangentialAccelVar(0);
        
        // emitter position
        m_emitter->setPosition( CCPointMake(0,0) );
        m_emitter->setPosVar(ccp(80 , 0));
        
        // life of particles
        m_emitter->setLife(10);
        m_emitter->setLifeVar(1);
        
        // spin of particles
        m_emitter->setStartSpin(0);
        m_emitter->setStartSpinVar(0);
        m_emitter->setEndSpin(0);
        m_emitter->setEndSpinVar(340);
        
        // color of particles
        ccColor4F startColor = {1.0f, 1.0f, 1.0f, 1.0f};
        m_emitter->setStartColor(startColor);
        
        ccColor4F startColorVar = {0.0f, 0.0f, 0.0f, 0.0f};
        m_emitter->setStartColorVar(startColorVar);
        
        ccColor4F endColor = {1.0f, 1.0f, 1.0f, 1.0f};
        m_emitter->setEndColor(endColor);
        
        ccColor4F endColorVar = {0.0f, 0.0f, 0.0f, 0.6f};
        m_emitter->setEndColorVar(endColorVar);
        
        // size, in pixels
        m_emitter->setStartSize(50.0f);
        m_emitter->setStartSizeVar(00.0f);
        m_emitter->setEndSize(kParticleStartSizeEqualToEndSize);
        
        // emits per second
        m_emitter->setEmissionRate(m_emitter->getTotalParticles()/m_emitter->getLife());
        
        // additive
        m_emitter->setBlendAdditive(false);
        
        return true;
    }
    
    FlowerDrop::~FlowerDrop()
    {
        if(m_emitter)
        {
            m_emitter->removeFromParentAndCleanup(true);
            m_emitter->release();
        }
    }
    
    void FlowerDrop::start()
    {
        m_emitter->scheduleUpdate();
    }
    
    void FlowerDrop::stop()
    {
        m_emitter->unscheduleUpdate();
    }
}