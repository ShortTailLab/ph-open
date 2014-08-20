#pragma once

#include "cocos2d.h"
#include "gfx/vec2.h"
//#include "Gem.h"
#include <vector>
#include <boost/shared_ptr.hpp>


namespace PH { // tolua_export
    
    // A generic grid class that encapsules common grid operations
    template<typename T>
    class Grid2D { // tolua_export
#if 0
        // tolua_begin
        TOLUA_TEMPLATE_BIND(T, PH::Gem*)
        // tolua_end
#endif
    public:
        class Array
        {
        public:
            enum Axis
            {
                XAxis,
                YAxis,
            };
            
        private:
            Grid2D<T>& grid;
            int pivot;
            Axis axis;
            
        public:
            Array(Grid2D<T>& g, int p, Axis a) : grid(g), pivot(p), axis(a) {}
            
            Vec2i coords(int itr) const
            {
                return axis == XAxis ? Vec2i(pivot, itr) : Vec2i(itr, pivot);
            }
            
            T operator() (int i)
            {
                return grid(coords(i));
            }
            
            int size()
            {
                return axis == XAxis ? grid.Height : grid.Width;
            }
        };
        
        std::vector<T> mGrid;
        
    public:
        const int Width; // tolua_export
        const int Height; // tolua_export
        
        Grid2D(int w, int h) : Width(w), Height(h)
        {
            mGrid.resize(Width * Height);
        }
        
        T& operator() (const Vec2i& p) { return mGrid[g2i(p[0], p[1])]; }
        const T& operator() (const Vec2i& p) const { return mGrid[g2i(p[0], p[1])]; }
        
        T& operator() (int x, int y) { return mGrid[g2i(x,y)]; }
        T& operator() (int i) { return mGrid[i]; }
        const T& operator() (int i) const { return mGrid[i]; }

        // tolua_begin
        T e(int x, int y) { return (*this)(x,y); }
        void e(int x, int y, T val){ (*this)(x,y) = val; }
        
        T e(int i) { return (*this)(i); }
        void e(int i, T val){ (*this)(i) = val; }
        // tolua_end

        int size() const { return mGrid.size(); } // tolua_export
        
        bool isInside(const Vec2& p) const
        {
            return p[0] < Width && p[0] >= 0 && p[1] < Height && p[1] >= 0;
        }
        
        Array column(int x)
        {
            return Array(*this, x, Array::XAxis);
        }
        
        Array row(int y)
        {
            return Array(*this, y, Array::YAxis);
        }
        
    private:
        // linear index to grid and vice versa
        int g2i(int x, int y) const {  return y * Width + x; }
        int g2i(const Vec2i& p) const {  return g2i(p[0], p[1]);  }
        Vec2i i2g(int i) const {  return Vec2i(i / Width, i % Width);  }
    }; // tolua_export
    
    class Gem;
    typedef boost::shared_ptr<Gem> GemPtr;
    typedef Grid2D<GemPtr> GemGrid;
    
    struct GridBlock
    {
        Vec2i o;
        Vec2i s;
    };
} // tolua_export