
#include "UIAnimation.h"
#include "Board/Utils.h"
#include "extensions/utils.h"

using namespace std;

namespace PH
{

CCAnimate *createHeroDisappearAnim()
{
	CCAnimation *animation = CCAnimation::create();
	animation->setDelayPerUnit(0.08);
	for(int i = 1 ; i < 13 ; i++){
		CCSprite *tex = GetSprite("gameui/qiu2_"+toStr(i) + ".png");
		animation->addSpriteFrameWithTexture(tex->getTexture() , tex->getTextureRect());
	}
	
	CCAnimate *anim = CCAnimate::create(animation);
	return anim;
}


CCAnimate *createHeroAppearAnim()
{
	CCAnimation *animation = CCAnimation::create();
	animation->setDelayPerUnit(0.08);
	for(int i = 1 ; i <= 11 ; i++){
			CCSprite *tex = GetSprite("gameui/qiu1_"+toStr(i) + ".png");
		animation->addSpriteFrameWithTexture(tex->getTexture() , tex->getTextureRect());
	}
	
	CCAnimate *anim = CCAnimate::create(animation);
	return anim;
}

}
