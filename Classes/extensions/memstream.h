//
//  memstream.h
//  HelloCpp
//
//  Created by Zeyang Li on 5/3/13.
//
//

#pragma once

#include <vector>

namespace STL
{
    class memstream
    {
    private:
        std::vector<char> mem;
    public:
        template <typename X>
        void write(const X& x);
        void write(const void* buffer, size_t size);
    };
    
    template <typename X>
    memstream& operator << (memstream& mem, const X& x)
    {
        mem.write(x);
        return mem;
    }
}
