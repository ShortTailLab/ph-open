//
//  Fonts.h
//  HelloCpp
//
//  Created by Zeyang Li on 12/27/12.
//
//

#pragma once

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#define FONT_PREFIX "ttf/"
#define FONT_POSTFIX ".ttf"
#else //if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#define FONT_PREFIX ""
#define FONT_POSTFIX ""
#endif

#define FONT_CN FONT_PREFIX "FZPangWa-M18S" FONT_POSTFIX
#define FONT_NUM FONT_PREFIX "DFPSumo-W12" FONT_POSTFIX

#define TITLE_YELLOW (ccc3(0xff, 0xe6, 0x00))
#define NUM_LIGHT_BLUE (ccc3(0xa7, 0xe5, 0xff))
