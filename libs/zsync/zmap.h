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

#include "zsglobal.h"

struct gzblock {
  uint16_t inbitoffset;
  uint16_t outbyteoffset;
} __attribute__((packed));

#define GZB_NOTBLOCKSTART 0x8000

struct zmap;
struct z_stream_s;

struct zmap* zmap_make(const struct gzblock* zb, int n);
void zmap_free(struct zmap*);

off_t* zmap_to_compressed_ranges(const struct zmap* zm, off_t* byterange, int nrange, int* num);
void configure_zstream_for_zdata(const struct zmap* zm, struct z_stream_s* zs, long zoffset, long long* poutoffset);

/* gzip flag byte */
#define GZ_ASCII_FLAG   0x01 /* bit 0 set: file probably ascii text */
#define GZ_HEAD_CRC     0x02 /* bit 1 set: header CRC present */
#define GZ_EXTRA_FIELD  0x04 /* bit 2 set: extra field present */
#define GZ_ORIG_NAME    0x08 /* bit 3 set: original file name present */
#define GZ_COMMENT      0x10 /* bit 4 set: file comment present */
#define GZ_RESERVED     0xE0 /* bits 5..7: reserved */

/* mtime is in bytes 4..7 of the gzip header */
static inline int zhead_has_mtime(const char* p) {
    return !!(p[4] || p[5] || p[6] || p[7]);
}

static inline int zhead_has_fname(const char* p) {
    return !!(p[3] & GZ_ORIG_NAME);
}

static inline const char* skip_zhead(const char* p)
{
    const char* q = p + 10;
    if (zhead_has_fname(p)) {
        q += strlen(q)+1;
    }

    return q;
}

