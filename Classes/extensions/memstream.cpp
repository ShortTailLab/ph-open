//
//  memstream.cpp
//  HelloCpp
//
//  Created by Zeyang Li on 5/3/13.
//
//

#include "memstream.h"

namespace STL
{
    template <typename X>
    void memstream::write(const X& x)
    {
        write(&x, sizeof(X));
    }
    
    void memstream::write(const void* buffer, size_t size)
    {
        int offset = mem.size();
        // resize to desired capcity and fill with 0
        mem.resize(offset + size, 0);
        memcpy(&mem + offset, buffer, size);
    }
}
