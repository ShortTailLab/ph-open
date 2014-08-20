#include "Console.h"


USING_NS_CC;
using namespace std;

cocos2d::CCLabelTTF *Console::board;
std::string Console::content;

void Console::reg(CCScene *scene)
{
	board = CCLabelTTF::create("hello world","Arial",33);
	board->setPosition( ccp(100 , 600));
	scene->addChild(board);
}

void Console::write(const string s)
{
	content += s + "\n";
	
	board->setString(content.c_str());
}
