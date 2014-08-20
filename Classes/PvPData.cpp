//
//  PvPData.cpp
//  HelloCpp
//
//  Created by zql on 13-5-30.
//
//

#include "PvPData.h"

namespace PH {
    
    
    PvPData* PvPData::getInstance()
    {
        static PvPData* o = new PvPData();
        return o;
    }
}
