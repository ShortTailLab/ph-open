
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

/* zmap part of libzsync
 * Random access for gzip files. Yes, really.
 * Code to read a zmap made by zsyncmake, map block ranges in the uncompressed
 * data to block ranges in the compressed data, and then to configure a zlib
 * zstream object to commence reading a compressed stream mid-stream.
 */

#include "zsglobal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#ifdef WITH_DMALLOC
# include <dmalloc.h>
#endif

#include "zmap.h"
#include "format_string.h"

/* This is a record of a checkpoint in a zlib stream - we have the bit position
 * (yes, bit - zlib compresses input bytes down to a variable number of bits)
 * and the corresponding output byte offset.
 * blockcount is 0 if this bit position in the zlib stream is the start of a
 *  zlib block, and is 1, 2, 3 etc for subsequent points that are in the same
 *  zlib block. */

struct zmapentry {
    long long inbits;
    long long outbytes;
    int blockcount;
};

/* Store all the zmapentry's as an array, and the # of entries */
struct zmap {
    int n;
    struct zmapentry *e;
};

/* zmap_make(gzblock[], numblocks)
 * Constructor. Supply the gzblocks (read from the Z-Map in the .zsync file)
 */
struct zmap *zmap_make(const struct gzblock *zb, int n) {
    int i;

    /* Entries in the gzblock format are relative. We want absolute offsets; so
     * here are the absolute position on the 'in' (compressed) and 'out'
     * (uncompressed) streams as state for the loop below. */
    long long in = 0;
    long long out = 0;
    int bc = 0;     /* And this is the number of map points see in the current zlib block */

    /* Allocate zmap, space for all its entries, fill in fields */
    struct zmap *m = malloc(sizeof(struct zmap));
    if (!m)
        return m;
    m->n = n;
    m->e = malloc(sizeof(struct zmapentry) * n);
    if (!m->e) {
        free(m);
        return NULL;
    }

    /* Convert the packed on-disk platform-neutral storage into our in-memory
     * native storage with absolute offsets. Entry-by-entry. */
    for (i = 0; i < n; i++) {
        uint16_t ob = ntohs(zb[i].outbyteoffset);

        /* Identify zlib block starts and adjust in-block count accordingly */
        if (ob & GZB_NOTBLOCKSTART) {
            ob &= ~GZB_NOTBLOCKSTART;
            bc++;
        }
        else {
            bc = 0;
        }

        /* Calculate absolute position of this map entry */
        in += ntohs(zb[i].inbitoffset);
        out += ob;

        /* And write the entry */
        m->e[i].inbits = in;
        m->e[i].outbytes = out;
        m->e[i].blockcount = bc;
    }
    return m;
}

/* zmap_free - destructor */
void zmap_free(struct zmap *m) {
    free(m->e);
    free(m);
}

/* consolidate_byteranges
 * The byte ranges in the compressed content determined as sufficient to return
 * each range in the uncompressed content could contain duplication/overlap.
 * So we go through and consolidate any overlapping ranges.
 */
static off_t* consolidate_byteranges(off_t* zbyterange, int* num) {
    int k = *num;
    int i;

    for (i = 0; i < k - 1;) {
        if (zbyterange[2 * i + 1] >= zbyterange[2 * (i + 1)]) {
            /* Ranges overlap, merge
            * The end of the first range need not be before the end of the
             *  second, so this if () block is to set the end of the combined block
             *  to the greater of the two.
             * The start of the second block could be before the start of the first:
             *  but this only occurs where the second range is a block header, and
             *  the first range is some data that an earlier uncompressed range
             *  needed out of the same block, in which case we are guaranteed that
             *  the block header must have been requested earlier, and so the second
             *  block here can be dropped anyway.
             */
            if (zbyterange[2 * i + 1] < zbyterange[2 * (i + 1) + 1])
                zbyterange[2 * i + 1] = zbyterange[2 * (i + 1) + 1];

            /* Now eliminate zbyterange[2*(i+1) +0 and +1]; */
            memmove(&zbyterange[2 * i + 2], &zbyterange[2 * i + 4],
                    2 * (k - 2 - i) * sizeof(zbyterange[0]));
            k--;
        }
        else
            i++;
    }
    /* Update the number of ranges with the new number, and fit the memory
     * allocation to the actual number of ranges it contains */
    *num = k;
    if (k > 0) {
        zbyterange = realloc(zbyterange, 2 * k * sizeof *zbyterange);
    }
    return zbyterange;
}

/* num_ranges = find_compressed_ranges_for(
 *      zmap, ranges[], num_ranges, &state, start_offset, end_offset)
 * Adds byte ranges to the supplied ranges structure (and returns the new total
 * number) such that the compressed content contained will certainly produce
 * the uncompressed content [start_offset, end_offset).
 *
 * &state is a long long that the caller provides to keep state between calls
 * (records the last block header that we added a range for, so we don't re-add
 * it again.) Returns -1 on error.
 *
 * Adds at most 2 byte ranges per call, and the caller is responsible for
 * ensuring that ranges[] has enough room for at least that.
 */
static int find_compressed_ranges_for(const struct zmap* zm, off_t* zbyterange,
        int k, long long* lastwroteblockstart_inbitoffset,
        long long start, long long end)
{
    int j;

    /* the zstart, zend vars are the main state for the loop below:
     *  zstart == -1: we are looking for the first compressed block containing
     *    data from our target range.
     *  zstart is an offset, zend == -1: we found the start, now looking for
     *    the first compressed block that is outside the range that we need.
     *  zstart, zend are offsets: we got the end of the block too, all done. */
    long long zstart = -1;
    long long zend = -1;

    /* This is the offset of the previous compressed block start. See comment
     * in the loop below for where/why we need it. */
    long long lastblockstart_inbitoffset = 0;

    /* Step through the blocks of compressed data */
    for (j = 0; j < zm->n && (zstart == -1 || zend == -1); j++) {
        register long long inbitoffset = zm->e[j].inbits;
        register long long outbyteoffset = zm->e[j].outbytes;

        /* Is this the first block that comes after the start point - if so, the
         * previous block is the place to start */
        if (start < outbyteoffset && zstart == -1) {
            if (j == 0)
                break;
            zstart = zm->e[j - 1].inbits;

            /* We need the zlib block header for range of compressed data
             * - you can't decompress the data without knowing the huffman tree
             * for this block of data.
             * So, immediately add a range of at least
             *  *** WARNING MAGIC NUMBER ***           200 bytes
             * (which is a guess by me, I think the zlib header never exceeds that)
             * covering the preceding zlib block header */

            if (*lastwroteblockstart_inbitoffset !=
                    lastblockstart_inbitoffset) {
                zbyterange[2 * k] = lastblockstart_inbitoffset / 8;
                zbyterange[2 * k + 1] = zbyterange[2 * k] + 200;
                k++;
                *lastwroteblockstart_inbitoffset = lastblockstart_inbitoffset;
            }
        }

        /* We need to remember the most recent zlib block header, for the above.
         * (Note this is after the section above, because the code above is
         *  looking at the previous checkpoint, zm->e[j-1]. Only now do we worry
         *  about data in zm->e[j] .)
         * If blockcount == 0, this point in the compressed data is a block header */
        if (zm->e[j].blockcount == 0) {     /* Block starts here */
            lastblockstart_inbitoffset = inbitoffset;
        }

        /* If we have passed the start, and we have now passed the end, then
         * the end of this block is the end of the range to fetch.  Special
         * case end of stream, where the range libzsync knows about could
         * extend beyond the range of the zlib stream. */
        if (start < outbyteoffset
                && (end <= outbyteoffset || j == zm->n - 1)) {
            zend = inbitoffset;
        }
    }

    /* If we failed to get either start or end, we're in trouble */
    if (zend == -1 || zstart == -1) {
        return -1;
    }

    /* Finally, translate bits to bytes and store these in our list of ranges
     * to get, and return the number of ranges to the caller so they know how
     * many they have now */
    zbyterange[2 * k] = zstart / 8;
    zbyterange[2 * k + 1] = (zend + 7) / 8;
    return k+1;
}

/* zmap_to_compressed_ranges(self, byteranges[], num_ranges, &num_zranges)
 * For each range of data that we want from the uncompressed file, work out a
 * corresponding byte range in the compressed file that definitely includes the
 * data in the target range. Return the byte ranges for the compressed file.
 *
 * Returns: byte ranges in the compressed file; and number of ranges in *num_zranges.
 * Ranges are an array of off_t[2*num_ranges], each byte range having 2 entries
 * for start and end.
 */
off_t *zmap_to_compressed_ranges(const struct zmap *zm, off_t * byterange,
                                 int nrange, int *num) {
    int i;
    long long lastwroteblockstart_inbitoffset = 0;

    /* Allocate enough space to contain the byte ranges in the compressed file.
     * Allocate more than we need and shrink to fit at the end -
     *  2 byte ranges (of 2 off_t each) per range is the limit. */
    off_t *zbyterange = malloc(2 * 2 * nrange * sizeof *byterange);
    int k = 0; /* The number of zbyterange entries we actually have so far (each of 2 off_t) */

    for (i = 0; i < nrange; i++) {
        /* (try to) Find byte ranges in the compressed file to get this the ith
         * byterange. */
        k = find_compressed_ranges_for(zm, zbyterange, k, &lastwroteblockstart_inbitoffset,
                                       byterange[2 * i], byterange[2 * i + 1]);
        if (k < 0) {
            fprintf(stderr, "Z-Map couldn't tell us how to find %lld-%lld\n", byterange[2 * i], byterange[2 * i + 1]);
            free(zbyterange);
            return NULL;
        }
    }

    /* Return the # of ranges and the array of byte ranges we have built 
     * after consolidating ranges where possible */
    *num = k;
    return consolidate_byteranges(zbyterange, num);
}

#include "zlib.h"
/* zmap_search(self, offset)
 * Find this offset in the Z-Map */
int zmap_search(const struct zmap* zm, long zoffset) {
    /* State for binary search */
    int low = 0;
    int high = zm->n - 1;

    while (low <= high) {
        int m = (low + high) / 2;
        long long inbyte = zm->e[m].inbits / 8;
        if (inbyte == zoffset) {
            low = high = m;
            break;
        }
        else if (zoffset < inbyte) {
            high = m - 1;
        }
        else {
            low = m + 1;
        }
    }
    if (low > high) {
        fprintf(stderr, "bad offset %ld, not in z-map\n", zoffset);
        exit(3);
    }
    return low;
}

#if SUPPORT_GZIP_STREAM
/* configure_zstream_for_zdata(self, zstream, offset, &poutoffset)
 * Given an zoffset and a zmap, configure the supplied zstream to be in the
 * correct state to interpret the compressed data stream read from the
 * compressed file at this offset. And return the offset in the uncompressed
 * stream that this corresponds to in the supplied long long* .
 * NOTE: the caller must call zlib:updatewindow() on the zstream to supply it
 * with 32k of leading context in the uncompressed stream, before the zstream
 * can be used to actually decompress.
 *
 * Requires some cooperation from the caller - it should not be called with an
 * offset that is not a block start unless it has been previously called with
 * an offset that is a block start, and the most recent block start call should
 * be for the same block as the current offset is contained in. If you call
 * this with the start offsets of blocks returned by zmap_to_compressed_ranges
 * and in the order that it returned them, this condition is satisfied.
 */
void configure_zstream_for_zdata(const struct zmap *zm, z_stream * zs,
                                 long zoffset, long long *poutoffset) {
    /* Find the zmap entry corresponding to this offset */
    int i = zmap_search(zm, zoffset);

    /* If this is a compressed block start (so a new block), restart the
     * decompression fresh at this point */
    if (!zm->e[i].blockcount) {
        /* Release any old inflate object */
        if (zs->total_in > 0)
            inflateEnd(zs);

        inflateInit2(zs, -MAX_WBITS);
    }

    /* Else, the stream should already be configured for decompressing this
     * block. Sanity check - this zstream should have read data (the block
     * header) already). */
    else if (zs->total_in == 0) {
        fprintf(stderr, "bad first offset %ld, not a block start.\n", zoffset);
        exit(3);
    }

    /* Work out what the decompressed data will correspond to */
    *poutoffset = zm->e[i].outbytes;

    /* Align with the bitstream */
    inflate_advance(zs, zoffset, zm->e[i].inbits % 8, !zm->e[i].blockcount);
}
#endif
