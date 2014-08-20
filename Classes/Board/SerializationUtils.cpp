//
//  SerializationUtils.cpp
//  HelloCpp
//
//  Created by 毛 靖凯 on 13-8-29.
//
//

#include "SerializationUtils.h"

namespace PH
{
    size_t ObjectGUIDGenerator()
    {
        static size_t count = 0;
        // it's not thread safety
        return count ++;
    }
}
