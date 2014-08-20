#ifndef GFX_INCLUDED // -*- C++ -*-
#define GFX_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Main header file for the libgfx graphics library.

  $Id: gfx.h 455 2005-08-17 18:10:25Z garland $

 ************************************************************************/

#if defined(HAVE_CONFIG_H)
#  include "gfx/config.h"
#elif defined(_MSC_VER)
#  include "gfx/config-vc.h"
#elif defined(__APPLE__)
#  include "gfx/config-osx.h"
#endif

#include <cstdlib>
#include <cmath>
#include <climits>
#include <iostream>

////////////////////////////////////////////////////////////////////////
//
// Standards notwithstanding, not all platforms define exactly the
// same things in their header files.  We try to compensate here.
//

// #if !defined(HAVE_BOOL)
// typedef int bool;
// const bool false = 0;
// const bool true = 1;
// #endif

// Microsoft doesn't define std::min() & std::max() because it conflicts with
// <windef.h>, and their _MIN/_MAX macro workarounds don't add the std::
// namespace qualification.  These macros provide a uniform way of getting
// around this problem.
//



#if defined(_MSC_VER)
	#define NOMINMAX
//#  define MIN(a,b) std::_cpp_min(a,b)
//#  define MAX(a,b) std::_cpp_max(a,b)
#else
#  if defined(__CYGWIN__)
#    define NOMINMAX
#  endif
#  undef MIN
#  undef MAX
#  define MIN(a,b) std::min(a,b)
#  define MAX(a,b) std::max(a,b)
#endif

#ifndef M_PI
#  define M_PI 3.14159265358979323846264338327950288419716939937510582097494459
#endif

#if !defined(HUGE) && defined(HUGE_VAL)
#  define HUGE HUGE_VAL
#endif

#if !defined(HAVE_RINT)
inline double rint(double x) { return floor(x + 0.5); }
#endif

////////////////////////////////////////////////////////////////////////
//
//
//
namespace gfx
{

#if defined(HAVE_RANDOM)
  inline double random1() { return (double)random() / (double)LONG_MAX; }
  inline char   random_byte() { return (char)(random() & 0xff); }
#else
  inline double random1() { return (double)rand() / (double)RAND_MAX; }
  inline char   random_byte() { return (char)(rand() & 0xff); }
#endif

const double FEQ_EPS = 1e-6;
const double FEQ_EPS2 = 1e-12;

inline bool  FEQ(double a, double b, double e=FEQ_EPS)  {return fabs(a-b)<e;}
inline bool FEQ2(double a, double b, double e=FEQ_EPS2) {return fabs(a-b)<e;}


////////////////////////////////////////////////////////////////////////
//
//
//

#define TIMING(t, cmd) { t=get_cpu_time(); cmd; t=get_cpu_time() - t; }
extern double get_cpu_time();

} // namespace gfx

#ifndef GFX_NAMESPACE
using namespace gfx;
#endif

// GFX_INCLUDED
#endif
