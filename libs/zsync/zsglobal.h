/*
 *   zsync - client side rsync over http
 *   Copyright (C) 2004,2005,2007,2009 Colin Phipps <cph@moria.org.uk>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the Artistic License v2 (see the accompanying
 *   file COPYING for the full license terms), or, at your option, any later
 *   version of the same license.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   COPYING file for details.
 */

#ifndef ZSGLOBAL_H
#define ZSGLOBAL_H


#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#if defined(__GNUC__) && defined (__OpenBSD__)
#  define ZS_DECL_BOUNDED(x,y,z) __attribute__((__bounded__(x,y,z)))
#else
#  define ZS_DECL_BOUNDED(x,y,z)
#endif /* ZS_DECL_BOUNDED */

/* Define to the full name of this package. */
#define PACKAGE_NAME "zsync"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "zsync 0.6.2"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "zsync"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.6.2"

/* Version number of package */
#define VERSION "0.6.2"

#define SUPPORT_GZIP_STREAM 0

#endif
