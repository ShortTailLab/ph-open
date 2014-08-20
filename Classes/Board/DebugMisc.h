//
//  DebugMisc.h
//  HelloCpp
//
//  Created by Zeyang Li on 12/7/12.
//
//

#pragma once

#ifndef PH_DEBUG_BUILD
    #define PH_DEBUG_BUILD 0
#endif

#define DISABLE_TUTORIAL (PH_DEBUG_BUILD && 1)

#define SKILL_NEED_NO_ENERGY (PH_DEBUG_BUILD && 1)

//#define PH_USE_BUNDLE_RESOURCE (PH_DEBUG_BUILD && 1)
#define PH_USE_BUNDLE_RESOURCE 1

#define PH_PVP_ALWAYS_ENABLED   (PH_DEBUG_BUILD && 0)
