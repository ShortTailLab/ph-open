/************************************************************************

 Visual C++ 5/6 configuration header.
 Manually generated from config.h.in.

 ************************************************************************/

/* Define this if your compiler doesn't support the new C++ 'bool' type */
#define HAVE_BOOL 1

/* Define if your system supports rint() */
/* #define HAVE_RINT 1 */

/* Define if your system supports getrusage() */
/* #undef HAVE_GETRUSAGE */

/* Define if your system does not support getrusage() but supports times() */
/* #undef HAVE_TIMES */

/* Define if your system supports random() as opposed to just rand() */
/* #undef HAVE_RANDOM */

/* Define if the STL hash_map template is available */
/* #undef HAVE_HASH_MAP */

/* Define if the STL hash_set template is available */
/*#undef HAVE_HASH_SET */

/* GCC v3 puts hash_map and hash_set in an 'ext' directory */
/* #undef HAVE_EXT_HASH_MAP */
/* #undef HAVE_EXT_HASH_SET */

/* Define if the STL valarray  template is available */
#define HAVE_VALARRAY 1

/* Define if the ANSI standard <sstream> is available */
#define HAVE_SSTREAM 1

/* Define if the pre-ANSI <strstream> is available */
/* #undef HAVE_STRSTREAM */

/* Define if the FLTK GUI toolkit is available */
#define HAVE_FLTK 1

/* Define if FLTK OpenGL support is available */
#define HAVE_FLTK_GL 1

/* Define if Sam Leffler's libtiff is available */
#define HAVE_LIBTIFF 1

/* Define if libpng is available */
#define HAVE_LIBPNG 1

/* Define if the Independent JPEG Group's libjpeg is available */
#define HAVE_LIBJPEG 1

/* ***** OpenGL configuration section ***** */

/* Define to the name of OpenGL implementation (e.g., "OpenGL" or "Mesa") */
#define HAVE_OPENGL "OpenGL"

/* Define if glPolygonOffsetEXT is available */
/* #undef HAVE_POLYOFFSET_EXT */

/* Define if glPolygonOffset is available */
#define HAVE_POLYOFFSET 1

/* Define if <GL/glext.h> header is available */
#define HAVE_GL_GLEXT_H 1

/* Define if <GL/glxext.h> header is available */
/* #undef HAVE_GL_GLXEXT_H */

/* Define if <GL/wglext.h> header is available */
#define HAVE_GL_WGLEXT_H 1

/* Visual C++ gets confused by too many casting and [] operators */
#define HAVE_CASTING_LIMITS 1

#if defined(_DEBUG) && defined(_MSC_VER)
// STL makes Visual C++ complain about identifiers longer than 255
// characters.  Unfortunately, this may limit the debugability of
// code that uses STL.
#pragma warning (disable : 4786)
#endif
