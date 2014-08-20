
/*
 *   rcksum/lib - library for using the rsync algorithm to determine
 *               which parts of a file you have and which you need.
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

/* Manage storage of the set of ranges in the target file that we have so far
 * got data for. */

#include "zsglobal.h"

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#ifdef WITH_DMALLOC
# include <dmalloc.h>
#endif

#include "rcksum.h"
#include "internal.h"

/* r = range_before_block(self, x)
 * This determines which of the existing known ranges x falls in.
 * It returns -1 if it is inside an existing range (it doesn't tell you which
 *  one; if you already have it, that usually is enough to know).
 * Or it returns 0 if x is before the 1st range;
 * 1 if it is between ranges 1 and 2 (array indexes 0 and 1)
 * ...
 * numranges if it is after the last range
 */
static int range_before_block(const struct rcksum_state* rs, zs_blockid x) {
    /* Lowest number and highest number block that it could be inside (0 based) */
    register int min = 0, max = rs->numranges-1;

    /* By bisection */
    for (; min<=max;) {
        /* Range number to compare against */
        register int r = (max+min)/2;

        if (x > rs->ranges[2*r+1]) min = r+1;  /* After range r */
        else if (x < rs->ranges[2*r]) max = r-1;/* Before range r */
            else return -1;                     /* In range r */
    }

    /* If we reach here, we know min = max + 1 and we were below range max+1
     * and above range min-1.
     * So we're between range max and max + 1
     * So we return max + 1  (return value is 1 based)  ( = min )
     */
    return min;
}

/* add_to_ranges(rs, blockid)
 * Mark the given blockid as known, updating the stored known ranges
 * appropriately */
void add_to_ranges(struct rcksum_state *rs, zs_blockid x) {
    int r = range_before_block(rs, x);

    if (r == -1) {
        /* Already have this block */
    }
    else {
        rs->gotblocks++;

        /* If between two ranges and exactly filling the hole between them,
         * merge them */
        if (r > 0 && r < rs->numranges
            && rs->ranges[2 * (r - 1) + 1] == x - 1
            && rs->ranges[2 * r] == x + 1) {

            // This block fills the gap between two areas that we have got completely. Merge the adjacent ranges
            rs->ranges[2 * (r - 1) + 1] = rs->ranges[2 * r + 1];
            memmove(&rs->ranges[2 * r], &rs->ranges[2 * r + 2],
                    (rs->numranges - r - 1) * sizeof(rs->ranges[0]) * 2);
            rs->numranges--;
        }

        /* If adjoining a range below, add to it */
        else if (r > 0 && rs->numranges && rs->ranges[2 * (r - 1) + 1] == x - 1) {
            rs->ranges[2 * (r - 1) + 1] = x;
        }

        /* If adjoining a range above, add to it */
        else if (r < rs->numranges && rs->ranges[2 * r] == x + 1) {
            rs->ranges[2 * r] = x;
        }

        else { /* New range for this block alone */
            rs->ranges =
                realloc(rs->ranges,
                        (rs->numranges + 1) * 2 * sizeof(rs->ranges[0]));
            memmove(&rs->ranges[2 * r + 2], &rs->ranges[2 * r],
                    (rs->numranges - r) * 2 * sizeof(rs->ranges[0]));
            rs->ranges[2 * r] = rs->ranges[2 * r + 1] = x;
            rs->numranges++;
        }
    }
#if 0
    {
        int i;
        for (i = 0; i < rs->numranges; i++)
            fprintf(stderr, "%d-%d,", rs->ranges[i * 2], rs->ranges[i * 2 + 1]);
        fprintf(stderr, " are the current ranges got (added %d, %d)\n\n", x, r);
    }
#endif
}

/* already_got_block
 * Return true iff blockid x of the target file is already known */
int already_got_block(struct rcksum_state *rs, zs_blockid x) {
    return (range_before_block(rs, x) == -1);
}

/* next_blockid = next_known_block(rs, blockid)
 * Returns the blockid of the next block which we already have data for.
 * If we know the requested block, it returns the blockid given; otherwise it
 * will return a later blockid.
 * If no later blocks are known, it returns rs->numblocks (i.e. the block after
 * the end of the file).
 */
zs_blockid next_known_block(struct rcksum_state *rs, zs_blockid x) {
    int r = range_before_block(rs, x);
    if (r == -1)
        return x;
    if (r == rs->numranges) {
        return rs->blocks;
    }
    /* Else return first block of next known range. */
    return rs->ranges[2*r];
}

/* rcksum_needed_block_ranges
 * Return the block ranges needed to complete the target file */
zs_blockid *rcksum_needed_block_ranges(const struct rcksum_state * rs, int *num,
                                       zs_blockid from, zs_blockid to) {
    int i, n;
    int alloc_n = 100;
    zs_blockid *r = malloc(2 * alloc_n * sizeof(zs_blockid));

    if (!r)
        return NULL;

    if (to >= rs->blocks)
        to = rs->blocks;
    r[0] = from;
    r[1] = to;
    n = 1;
    /* Note r[2*n-1] is the last range in our prospective list */

    for (i = 0; i < rs->numranges; i++) {
        if (rs->ranges[2 * i] > r[2 * n - 1])
            continue;
        if (rs->ranges[2 * i + 1] < from)
            continue;

        /* Okay, they intersect */
        if (n == 1 && rs->ranges[2 * i] <= from) {       /* Overlaps the start of our window */
            r[0] = rs->ranges[2 * i + 1] + 1;
        }
        else {
            /* If the last block that we still (which is the last window end -1, due
             * to half-openness) then this range just cuts the end of our window */
            if (rs->ranges[2 * i + 1] >= r[2 * n - 1] - 1) {
                r[2 * n - 1] = rs->ranges[2 * i];
            }
            else {
                /* In the middle of our range, split it */
                r[2 * n] = rs->ranges[2 * i + 1] + 1;
                r[2 * n + 1] = r[2 * n - 1];
                r[2 * n - 1] = rs->ranges[2 * i];
                n++;
                if (n == alloc_n) {
                    zs_blockid *r2;
                    alloc_n += 100;
                    r2 = realloc(r, 2 * alloc_n * sizeof *r);
                    if (!r2) {
                        free(r);
                        return NULL;
                    }
                    r = r2;
                }
            }
        }
    }
    r = realloc(r, 2 * n * sizeof *r);
    if (n == 1 && r[0] >= r[1])
        n = 0;

    *num = n;
    return r;
}

/* rcksum_blocks_todo
 * Return the number of blocks still needed to complete the target file */
int rcksum_blocks_todo(const struct rcksum_state *rs) {
    int i, n = rs->blocks;
    for (i = 0; i < rs->numranges; i++) {
        n -= 1 + rs->ranges[2 * i + 1] - rs->ranges[2 * i];
    }
    return n;
}
