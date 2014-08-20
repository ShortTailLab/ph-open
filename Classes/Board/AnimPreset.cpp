//
//  AnimPreset.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 12/4/12.
//
//

#include "Board/AnimPreset.h"
#include "Animation/CCCallLambda.h"
#include "Board/Utils.h"
#include "Board/Common.h"
#include "Fonts.h"
#include <boost/bind.hpp>
#include <extensions/utils.h>

namespace PH
{
    boost::function<void()> removeGen(CCNode* n)
    {
        assert(n != NULL && "parameter is null");
        return [n]() { n->removeFromParentAndCleanup(true); };
    }
    
    static boost::function<void()> showGen(CCNode* n)
    {
        assert(n != NULL && "parameter is null");
        return [n]() { n->setVisible(true); };
    }
    
    float calOrbDelay(const CCPoint& src, const CCPoint& dst)
    {
        //        const float speed = 1/1800.0f;
        const float speed = 1/1800.0f;
        
        Vec2f p0(src.x, src.y);
        Vec2f p2(dst.x, dst.y);
        Vec2f dir = p2 - p0;
        
        float degree = M_PI*0.5f;
        
        Vec2f perp(cosf(degree) * dir[0] + -sinf(degree) * dir[1],
                   sinf(degree) * dir[0] +  cosf(degree) * dir[1]);
        if(perp[1] < 0.0f)
            perp *= -1.0f;
        perp *= 0.5f;
        
        // compute curve length
        float dis = norm(perp) * degree * 2;
        float time = dis * speed;
        
        return time;
    }
    
    TaskPtr createOrb(CCNode* parent, CCPoint src, CCPoint dst, bool delay)
    {
        CCSprite* orb = GetSprite("effects/generic_orb.png");
        orb->setVisible(false);
        orb->setPosition(src);
        
//        CCParticleSystemQuad* orb = CCParticleSystemQuad::create("particle/fire_ball.plist");
//        orb->setPosition(src);
//        orb->setVisible(false);
        
        parent->addChild(orb, ORDER_ORB);
        
//        const float speed = 1/1800.0f;
        const float speed = 1/1800.0f;
        
        Vec2f p0(src.x, src.y);
        Vec2f p2(dst.x, dst.y);
        Vec2f dir = p2 - p0;
        
        float degree = M_PI*0.5f;
        
        Vec2f perp(cosf(degree) * dir[0] + -sinf(degree) * dir[1],
                   sinf(degree) * dir[0] +  cosf(degree) * dir[1]);
        if(perp[1] < 0.0f)
            perp *= -1.0f;
        perp *= 0.5f;
        
        Vec2f p1 = perp + (p0 + p2)*0.5f;
                   
        // compute curve length
        float dis = norm(perp) * degree * 2;
        float time = dis * speed;
        
        ccBezierConfig bz = ccbz(ccp(p0[0], p0[1]), ccp(p1[0], p1[1]), ccp(p2[0], p2[1]));
        CCBezierBy* bzMove =  CCBezierTo::create(time, bz);

//        CCCatmullRomBy* bzMove = CCCatmullRomBy::create(time, cr);
        
        CCFiniteTimeAction * orbSeq = CCSequence::create(CCCallLambda::create([orb](){orb->setVisible(true);}),
                                                         bzMove,
                                                         CCCallLambda::create([orb](){orb->removeFromParentAndCleanup(false);}),
//                                                         CCCallLambda::create([=](){ orb->stopSystem(); }),
                                                         NULL);
        return TaskSequence::make()
        << TaskAnim::make(orb, orbSeq, delay);
    }
    
    TaskPtr createFloatWordText(CCNode* parent,
                                const std::string & str,
                                CCPoint src,
                                float delay)
    {
        // damage counter animation
        CCLabelBMFont* text = CCLabelBMFont::create(str.c_str(),
                                                    "bmfont/FloatWord_32.fnt");
        text->setOpacity(0);
        // use content scale. BM font is not automatically scaled
        text->setScale(contentScale()*1.05f);
        text->setPosition(src);
        
        parent->addChild(text, ORDER_FLOAT);
        
        // random roam destinations
        
        auto prev = CCSpawn::create(AnimPair(CCScaleBy::create(0.2, contentScale()*1.35f)),
                                    CCFadeIn::create(0.5f), NULL);
        auto act = CCSpawn::create(CCMoveBy::create(1.f, ccp(0, 100)),
                                   CCScaleTo::create(0.3f, contentScale()*1.05),
                                   CCFadeOut::create(1.3f),
                                   NULL);
        
        CCFiniteTimeAction* floatSeq = CCSequence::create(CCDelayTime::create(delay),
                                                          prev,
                                                          act,
                                                          CCCallLambda::create(removeGen(text)),
                                                          NULL);
        
        
        return TaskSequence::make()
        << createDelay(parent, delay)
        << TaskAnim::make(text, floatSeq, false);
    }
    
    TaskPtr createFloatText(CCNode* parent,
                            int val,
                            CCPoint src,
                            GemUtils::GemColor color,
                            float delay)
    {
        // damage counter animation
        CCLabelBMFont* text = CCLabelBMFont::create(toStr(val).c_str(),
                                                    GemUtils::numberFont(color));
        text->setOpacity(0);
        // use content scale. BM font is not automatically scaled
        text->setScale(contentScale());
        text->setPosition(src);
        
        parent->addChild(text, ORDER_FLOAT);
        
        // random roam destinations
        CCPoint randomBand = ccRandomBand(30, 40);
        CCPoint roamDst = ccp(src.x + randomBand.x, src.y + randomBand.y);
        
        CCFiniteTimeAction* floatSeq = CCSequence::create(CCDelayTime::create(delay),
                                                          CCFadeIn::create(0.3),
                                                          CCMoveTo::create(1.0f, roamDst),
                                                          CCFadeOut::create(0.15),
                                                          CCCallLambda::create(removeGen(text)),
                                                          NULL);
        return TaskAnim::make(text, floatSeq, false);
    }
    
    TaskPtr createEnergyText(CCNode* parent, int val, CCPoint src)
    {
        std::string s = val > 0.0f ? "+" + toStr(val) : toStr(val);
        // damage counter animation
        CCLabelBMFont* text = CCLabelBMFont::create(s.c_str(),
                                                    "bmfont/Molot_32_Energy.fnt");
        text->setOpacity(0);
        // use content scale. BM font is not automatically scaled
        text->setScale(contentScale()*0.8f);
        text->setPosition(src);
        text->setAnchorPoint(ccp(1.0, 0.0f));
        
        parent->addChild(text, 999);
        
        CCFiniteTimeAction* floatSeq = CCSequence::create(CCFadeIn::create(0.1f),
                                                          CCDelayTime::create(0.8f),
                                                          CCFadeOut::create(0.1f),
                                                          CCCallLambda::create(removeGen(text)),
                                                          NULL);
        return TaskAnim::make(text, floatSeq, false);
    }
    
    TaskPtr createTwoPhaseFloatText(CCNode* parent,
                                    const int phase1Val,
                                    const int phase2Val,
                                    const CCPoint src,
                                    const GemUtils::GemColor color,
                                    const float delay)
    {
        // damage counter animation
        CCLabelBMFont* text = CCLabelBMFont::create(toStr(phase1Val).c_str(),
                                                    GemUtils::numberFont(color));
        text->setOpacity(0);
        // use content scale. BM font is not automatically scaled
        text->setScale(contentScale());
        text->setPosition(src);
        
        parent->addChild(text, ORDER_FLOAT);
        
        // random roam destinations
        CCPoint rd1 = ccRandomBand(25, 35);
        rd1.y = std::abs(rd1.y);
        CCPoint roamDst1 = ccp(src.x + rd1.x, src.y + rd1.y);
        
        CCPoint rd2 = ccRandomBand(20, 21);
        rd2.y = phase1Val*0.5 > phase2Val ? -std::abs(rd2.y) : std::abs(rd2.y);
        CCPoint roamDst2 = ccp(roamDst1.x + rd2.x, roamDst1.y + rd2.y);
        
        float scale = 1.0f;
        if(phase2Val > phase1Val)
            scale = 1.35f;
        else if(phase2Val < phase1Val)
            scale = std::max((float)phase2Val / phase1Val, 0.6f);
        
        CCFiniteTimeAction* floatSeq =
            CCSequence::create(CCDelayTime::create(delay),
                               // phase1 animation
                               CCFadeIn::create(0.2),
                               CCMoveTo::create(0.7f, roamDst1),
                               // phase2 animation
                               CCSpawn::create(CCMoveTo::create(0.8f, roamDst2),
                                               CCScaleTo::create(0.8f, contentScale()*scale),
                                               CCCountBMTextNumber::create(phase1Val, phase2Val, 0.4),
                                               NULL),
                               CCFadeOut::create(0.15),
                               CCCallLambda::create([text](){text->removeFromParentAndCleanup(true);}),
                               NULL);
        
        return TaskAnim::make(text, floatSeq, false);
    }
    
    CCSprite* createComboTextLabel(const int val, const CCPoint src)
    {
        CCSprite* comboBg = GetSprite("ui/combo_80.png");
        comboBg->setPosition(src);
        comboBg->setAnchorPoint(ccp(0.5f, 0.8f));
        comboBg->setVisible(false);
        
        CCLabelBMFont* comboText = CCLabelBMFont::create(toStr(val).c_str(),
                                                         "bmfont/Combo_36.fnt");
        comboText->setTag(1);
        comboText->setPosition(ccp(comboBg->getContentSize().width * 0.5f, 34));
        comboText->setAnchorPoint(ccp(0.5, 0.0));
        comboBg->addChild(comboText);
        
        return comboBg;
    }
        
//    CFiniteTimeAction* createFadeThenRemove(Gem* gem, float time)
//    {
//        CCFadeOut* fadeOut = CCFadeOut::create(time);
//        CCCallLambda* remove = CCCallLambda::create([gem](){ gem->root->removeFromParentAndCleanup(true); });
//        CCFiniteTimeAction* seq = CCSequence::create(fadeOut, remove, NULL);
//        return seq;
//    }
    
    CCFiniteTimeAction* createIconJumpSequence(CCPoint p, float delay)
    {
        CCFiniteTimeAction* jump = CCSequence::create(CCDelayTime::create(delay),
//                                                      CCEaseElasticInOut::create(CCJumpTo::create(1.0f, p, 50, 1), 1.0f),
//                                                      CCEaseBackInOut::create(CCJumpTo::create(0.4f, p, 60, 1)),
                                                      CCJumpTo::create(0.4f, p, 60, 1),
                                                      NULL);
        return jump;
    }
        
    CCSprite* createShadowGem(GemUtils::GemColor c)
    {
        CCSprite* gem = GetSprite(GemUtils::res(c));
        gem->setAnchorPoint(ccp(0.5f, 0.5f));
        gem->setOpacity(125);
        
        return gem;
    }
    
    TaskPtr createAttackAnim(CCLayer* parent, CCPoint p, GemUtils::GemColor c)
    {
        CCAnimation* anim = CCAnimationCache::sharedAnimationCache()->animationByName(GemUtils::attackAnim(c));
        
        assert(anim != NULL && "anim is NULL, animations are not loaded?");
        
        // MUST use CCSprite. CCNode does not work with renderable animations
        CCSprite* effect = CCSprite::create();
        effect->retain();
        
        p.x += 10;
        
        if(c == GemUtils::Dark)
            p.y -= 20;
        else if(c == GemUtils::Light)
        {
            p.y += 10;
        }
        else if(c == GemUtils::Water)
        {
            p.x += 5;
        }
        
        effect->setPosition(p);
        
        if(c == GemUtils::Fire)
            effect->setScale(1.15);
        else
            effect->setScale(1.3);
        
        TaskSequencePtr seq = TaskSequence::make();
        
        seq->add(TaskLambda::make([=]()
        {
            parent->addChild(effect);
            ring(GemUtils::attackSound(c));
        }));
        seq->add(TaskAnim::make(effect, CCAnimate::create(anim)));
        seq->add(TaskLambda::make([=]()
        {
            effect->removeFromParentAndCleanup(true);
            effect->release();
        }));
        
        return seq;
    }
    
    CCFiniteTimeAction* createNumberAnimSeq(float from, float to, float duration)
    {
        duration = std::max(0.1f, duration);
        
        CCFiniteTimeAction* count = CCCountBMTextNumber::create(from, to, duration);
        
        float scale = contentScale() * 1.0f;
        float scaleBig = contentScale() * 1.8f;
    
        CCFiniteTimeAction* scaleUp = CCScaleTo::create(duration*0.5, scaleBig);
        CCFiniteTimeAction* scaleDown = CCScaleTo::create(duration*0.5, scale);
        CCFiniteTimeAction* scaleSeq = CCSequence::create(scaleUp,
                                                          scaleDown,
                                                          NULL);
        
        CCFiniteTimeAction* jump = CCJumpBy::create(duration, ccp(0, 0), 15, 1);
        
        return CCSpawn::create(count, scaleSeq, jump, NULL);
    }
    
    TaskPtr createDelay(CCNode* parent, float t)
    {
        CCNode* node = CCNode::create();
        CCDelayTime* delay = CCDelayTime::create(t);
        
        parent->addChild(node, -1000);
        
        return TaskSequence::make(TaskAnim::make(node, delay),
                                  TaskLambda::make([node](){ node->removeFromParentAndCleanup(true); }));
    }
    
    TaskPtr createPropertyAnim(StatusLabelPtr propertyLabel)
    {
        CCNode* l = propertyLabel->getRoot();
        // NOTE: property anim must because it's really easy to overwrite earlier anims
        // in different batches and we get a lost action blocking everything.
        l->stopAllActions();
        l->setScale(1.f);
        
        
        CCSprite * medium = GetSprite("fadong_01.png");
        
        l->addChild(medium, -100);
        medium->setOpacity(0);
        
        auto seq = CCSequence::create(CCScaleTo::create(0.35f, 1.3f),
                                      CCScaleTo::create(0.35f, 1.0f),
                                      NULL);
        
        /*
        auto medium = CCSprite::create("fadong_01.png");
        auto anim =
        CCAnimationCache::sharedAnimationCache()->animationByName("skill_bomb_fuze");
        medium->retain();
        auto act = CCSequence::create(CCAnimate::create(anim),
                                      CCCallLambda::create([=]()
                                      {
                                          medium->release();
                                      }), NULL);
        */
        return
        TaskSequence::make()
        //<< TaskAnim::make(medium, act, false)
        << TaskLambda::make([=]()
        {
            auto anim = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("skill_trigger"));
            medium->setOpacity(175);
            medium->runAction(CCSequence::create(anim,
                                                 CCCallLambda::create([=]()
                                                 {
                                                     medium->removeFromParent();
                                                 }), NULL));
        })
        << TaskAnim::make(l, seq, false);
    }
    
    TaskPtr createComboTextFadeRemove(CCSprite* comboText, int index)
    {
        /*
        auto batch = TaskBatch::make();
        
        float base = 1.3f + ((float)index)*0.12f;
        base = std::min(1.6f, base);
        
        auto prev = CCSpawn::create(CCEaseElasticOut::create(CCScaleTo::create(0.5f, base+0.2f)),
                                    CCFadeIn::create(0.5f), NULL);
        auto mid = CCSpawn::create(CCMoveBy::create(0.5f, ccp(0, 50)),
                                   CCScaleTo::create(0.3f, 1.3f),
                                   CCFadeOut::create(0.8f),
                                   NULL);
        
        CCFiniteTimeAction* act = CCSequence::create(prev,
                                                     mid,
                                                     CCCallLambda::create([comboText]()
                                                     {
                                                        comboText->removeFromParentAndCleanup(true);
                                                     }),
                                                     NULL);
        batch << TaskAnim::make(comboText, act, false);
        
        
        CCNode * parent = comboText->getParent();
        CCSprite * num = (CCSprite*)comboText->getChildByTag(1);
        if( parent != NULL && num != NULL )
        {
            auto seq = TaskSequence::make();
            
            auto num_act = CCSequence::create(CCDelayTime::create(0.5f),
                                              CCFadeOut::create(1.3f),
                                              NULL);
            seq << TaskAnim::make(parent, num_act, false);
            
            batch << seq;
        }
        
        
        return batch;
        */
        auto act = CCSequence::create(CCCallLambda::create(
                                      [=]()
                                      {
                                          comboText->getParent()->reorderChild(comboText, ORDER_FLOAT);
                                      }),
                                      AnimPair(CCScaleBy::create(0.2, 1.3)),
                                      CCSpawn::create(CCScaleTo::create(0.15, 1.5),
                                                      CCFadeOut::create(0.15),
                                                      NULL),
                                      CCCallLambda::create([comboText]()
                                      {
                                          comboText->removeFromParentAndCleanup(true);
                                      }),
                                      NULL);
        return TaskAnim::make(comboText, act);
    }
    
    TaskPtr moveGemAnimSlow(CCSprite* sprite, Vec2i gridP)
    {
        const int GEM_MOVE_TAG = 0x1011;
        
        TaskSequencePtr seq = TaskSequence::make();
        CCFiniteTimeAction* moveTo = CCEaseElasticOut::create(CCMoveTo::create(0.6f,
                                                                               g2w_center(gridP)),
                                                              1.2f);
    
        CCFiniteTimeAction* breath = CCSequence::create(CCFadeTo::create(0.2, 190),
                                                        CCFadeTo::create(0.2, 255),
                                                        NULL);
        CCFiniteTimeAction* both = CCSpawn::create(moveTo, breath, NULL);

        both->setTag(GEM_MOVE_TAG);
        
        seq << TaskLambda::make([=]()
        {
            CCAction* x = sprite->getActionByTag(GEM_MOVE_TAG);
            if(x)
                sprite->stopAction(x);
        });
        seq << TaskAnim::make(sprite, both, false);
        return seq;
    }
    
    TaskPtr moveGemAnim(CCSprite* sprite, Vec2i gridP)
    {
        const int GEM_MOVE_TAG = 0x1011;
        
        TaskSequencePtr seq = TaskSequence::make();
        CCFiniteTimeAction* moveTo = CCEaseElasticOut::create(CCMoveTo::create(0.2f,
                                                                               g2w_center(gridP)),
                                                              1.2f);
        
        moveTo->setTag(GEM_MOVE_TAG);
        seq << TaskLambda::make([=]()
                                {
                                    CCAction* x = sprite->getActionByTag(GEM_MOVE_TAG);
                                    if(x)
                                        sprite->stopAction(x);
                                });
        seq << TaskAnim::make(sprite, moveTo, false);
        return seq;
    }
    
    CCFiniteTimeAction* createShake(float t, CCPoint p, float rmin, float rmax)
    {
        auto rx = boost::bind(randf, rmin, rmax);
        
        return CCSequence::create(CCMoveTo::create(t*0.5f, ccp(p.x+rx(), p.y+rx())),
                                                    CCMoveTo::create(t, ccp(p.x-rx(), p.y-rx())),
                                                    CCMoveTo::create(t, ccp(p.x+rx(), p.y+rx())),
                                                    CCMoveTo::create(t, ccp(p.x-rx(), p.y-rx())),
                                                    CCMoveTo::create(t, ccp(p.x+rx(), p.y+rx())),
                                                    CCMoveTo::create(t, ccp(p.x-rx(), p.y-rx())),
                                                    CCMoveTo::create(t, ccp(p.x+rx(), p.y+rx())),
                                                    CCMoveTo::create(t, ccp(p.x-rx(), p.y-rx())),
                                                    CCMoveTo::create(t*0.5f, ccp(p.x, p.y)),
                                                    NULL);
        
    }
    
    CCAnimation* createLoadingCharacter()
    {
        CCAnimation* anim = CCAnimationCache::sharedAnimationCache()->animationByName("loading_anim");
        if(anim == NULL)
        {
            anim = CCAnimation::create();
            
            int numFrames = 4;
            for(int i=1; i<=numFrames; ++i)
            {
                char name[128];
                sprintf(name, "loading_anim%d.png", i);
                CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(name);
                
                assert(frame != NULL && "cannot find frames");
                
                anim->addSpriteFrame(frame);
            }
            
            anim->setDelayPerUnit(0.18);
            
            CCAnimationCache::sharedAnimationCache()->addAnimation(anim, "loading_anim");
        }
        
        return anim;
    }
    
    
    TaskPtr createVictoryAnim(CCNode* parent, const CCPoint& p)
    {
        CCSprite* brass = GetSprite("ui/shengli_wenzi.png");
        brass->setPosition(ccp(220, 100));
        
        CCSprite* swordLeft = GetSprite("ui/baojian_faguang.png");
        swordLeft->setAnchorPoint(ccp(0.5, 0.01));
        swordLeft->setPosition(ccp(88, 0));
        swordLeft->setOpacity(0);
        
        CCSprite* swordRight = GetSprite("ui/baojian_faguang.png");
        swordRight->setAnchorPoint(ccp(0.5, 0.01));
        swordRight->setPosition(ccp(440-88, 0));
        swordRight->setOpacity(0);
        
        CCSprite* ribbon = GetSprite("ui/shengli_sidai.png");
        //    ribbon->setPosition(ccp(320, 800));
        ribbon->setPosition(p);
        
        ribbon->addChild(swordLeft);
        ribbon->addChild(swordRight);
        ribbon->addChild(brass);
        
        ribbon->setVisible(false);
        ribbon->setScale(0.0f);
        
        parent->addChild(ribbon, ORDER_VICTORY);
        
        TaskSequencePtr seq = TaskSequence::make();
        
        seq << TaskSound::make("sound/v/victory.mp3");
        seq << TaskAnim::make(ribbon, CCSequence::create(CCShow::create(),
                                                         CCEaseExponentialOut::create(CCScaleTo::create(0.5, 1.0)),
                                                         NULL));
        
        TaskBatchPtr batch = TaskBatch::make();
        batch << TaskAnim::make(swordLeft, CCSpawn::create(CCFadeIn::create(0.1),
                                                           CCRotateBy::create(0.3, 45),
                                                           NULL));
        batch << TaskAnim::make(swordRight, CCSpawn::create(CCFadeIn::create(0.1),
                                                            CCRotateBy::create(0.3, -45),
                                                            NULL));
        seq << batch;
        return seq;
    }
    
    
    TaskPtr createDefeatAnim(CCNode* parent, const CCPoint& p)
    {
        CCSprite* brass = GetSprite("ui/baibei_wenzi.png");
        brass->setPosition(ccp(220, 100));
        
        CCSprite* swordLeft = GetSprite("ui/baojian_faguang.png");
        swordLeft->setAnchorPoint(ccp(0.5, 0.99));
        swordLeft->setPosition(ccp(88, 0));
        swordLeft->setScaleY(-1);
        swordLeft->setRotation(45);
        swordLeft->setOpacity(0);
        
        CCSprite* swordRight = GetSprite("ui/baojian_faguang.png");
        swordRight->setAnchorPoint(ccp(0.5, 0.99));
        swordRight->setPosition(ccp(440-88, 0));
        swordRight->setScaleY(-1);
        swordRight->setRotation(-45);
        swordRight->setOpacity(0);
        
        CCSprite* ribbon = GetSprite("ui/baibei_sidai.png");
//        ribbon->setPosition(ccp(320, 800));
        ribbon->setPosition(p);
        
        ribbon->addChild(swordLeft);
        ribbon->addChild(swordRight);
        ribbon->addChild(brass);
        
        ribbon->setVisible(false);
        
        ribbon->setScale(0.0f);
        ribbon->setTag(DEFEAT_ANIM_TAG);
        
        parent->addChild(ribbon, ORDER_VICTORY);
        
        TaskSequencePtr seq = TaskSequence::make();
        
        seq << TaskSound::make("sound/v/defeat.mp3");
        seq << TaskAnim::make(ribbon, CCSequence::create(CCShow::create(),
                                                         CCEaseExponentialOut::create(CCScaleTo::create(0.5, 1.0)),
                                                         NULL));
        
        TaskBatchPtr batch = TaskBatch::make();
        batch << TaskAnim::make(swordLeft, CCSpawn::create(CCFadeIn::create(0.2),
                                                           CCRotateBy::create(0.3, 35),
                                                           NULL));
        batch << TaskAnim::make(swordRight, CCSpawn::create(CCFadeIn::create(0.3),
                                                            CCRotateBy::create(0.3, -35),
                                                            NULL));
        seq << batch;
        return seq;    
    }
    
    TaskPtr createFlyin(CCNode* node, float duration)
    {
        CCPoint src(640 + node->getContentSize().width * node->getAnchorPoint().x, 480);
        CCPoint dst(  0 - node->getContentSize().width * node->getAnchorPoint().x, 480);
        
        node->setPosition(src);
        
        CCFiniteTimeAction* flyin =
            CCSequence::create(CCEaseElasticOut::create(CCMoveTo::create(duration, ccp(320, 480)), 0.8f),
                               CCEaseElasticIn::create(CCMoveTo::create(duration, dst), 0.8f),
                               NULL);
        return TaskAnim::make(node, flyin);
    }
}