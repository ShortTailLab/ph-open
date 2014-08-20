#pragma once
#include "cocos2d.h"
#include <iostream>
#include <string.h>

class Console
{
public:
	
	static void reg(cocos2d::CCScene *scene);
	static void write(const std::string s);

private:
	static cocos2d::CCLabelTTF *board; 
	static std::string content;

};

