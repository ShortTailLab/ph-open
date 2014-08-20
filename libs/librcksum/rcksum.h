/*
 *   rcksum/lib - library for using the rsync algorithm to determine
 *               which parts of a file you have and which you need.
 *   Copyright (C) 2004,2005,2009 Colin Phipps <cph@moria.org.uk>
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

/* This is the library interface. Very changeable at this stage. */

#include <stdio.h>

struct rcksum_state;

typedef int zs_blockid;

struct rsum {
	unsigned short	a;
	unsigned short	b;
} __attribute__((packed));

#define CHECKSUM_SIZE 16

struct rcksum_state* rcksum_init(zs_blockid nblocks, size_t blocksize, int rsum_butes, int checksum_bytes, int require_consecutive_matches);
void rcksum_end(struct rcksum_state* z);

/* These transfer out the filename and handle of the file backing the data retrieved.
 * Once you have transferred out the file handle, you can no longer read and write data through librcksum - it has handed it over to you, and can use it no more itself.
 * If you transfer out the filename, you are responsible for renaming it to something useful. If you don't transfer out the filename, librcksum will unlink it at rcksum_end.
 */
char* rcksum_filename(struct rcksum_state* z);
int rcksum_filehandle(struct rcksum_state* z);

void rcksum_add_target_block(struct rcksum_state* z, zs_blockid b, struct rsum r, void* checksum);

int rcksum_submit_blocks(struct rcksum_state* z, const unsigned char* data, zs_blockid bfrom, zs_blockid bto);
int rcksum_submit_source_data(struct rcksum_state* z, unsigned char* data, size_t len, off_t offset);
int rcksum_submit_source_file(struct rcksum_state* z, FILE* f, int progress);

/* This reads back in data which is already known. */
int rcksum_read_known_data(struct rcksum_state* z, unsigned char* buf, off_t offset, size_t len);

/* rcksum_needed_block_ranges tells you what blocks, within the given range,
 * are still unknown. It returns a list of block ranges in r[]
 * (at most max ranges, so spece for 2*max elements must be there)
 * these are half-open ranges, so r[0] <= x < r[1], r[2] <= x < r[3] etc are needed */
zs_blockid* rcksum_needed_block_ranges(const struct rcksum_state* z, int* num, zs_blockid from, zs_blockid to);
int rcksum_blocks_todo(const struct rcksum_state*);

/* For preparing rcksum control files - in both cases len is the block size. */
struct rsum __attribute__((pure)) rcksum_calc_rsum_block(const unsigned char* data, size_t len);
void rcksum_calc_checksum(unsigned char *c, const unsigned char* data, size_t len);

