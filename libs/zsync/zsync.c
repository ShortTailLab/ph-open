
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

/* This is the heart of zsync.
 *
 * .zsync file parsing and glue between all the main components of zsync.
 *
 * This file is where the .zsync metadata format is understood and read; it
 * extracts it and creates the corresponding rcksum object to apply the rsync
 * algorithm in constructing the target. It applies the zmap to convert byte
 * ranges between compressed and uncompressed versions of the data as needed,
 * and does decompression on compressed data received. It joins the HTTP code
 * to the rsync algorithm by converting lists of blocks from rcksum into lists
 * of byte ranges at particular URLs to be retrieved by the HTTP code.
 *
 * It also handles:
 * - blocking edge cases (decompressed data not lining up with blocks for rcksum; 
 *   last block of the file only containing partial data)
 * - recompression of the compressed data at the end of the transfer;
 * - checksum verification of the entire output.
 */
#include "zsglobal.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include <arpa/inet.h>

#ifdef WITH_DMALLOC
# include <dmalloc.h>
#endif

#include "zlib.h"

#include "librcksum/rcksum.h"
#include "zsync.h"
#include "zmap.h"

#include "platform/CCPlatformConfig.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#  define COMMON_DIGEST_FOR_OPENSSL
#  include <CommonCrypto/CommonDigest.h>
#else
#  include "openssl/sha.h"
#endif

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

#include <android/log.h>

#define  LOG_TAG    "puzzlehero"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

#else

#define  LOGD(...)  fprintf(stderr, __VA_ARGS__)

#endif

/* Probably we really want a table of compression methods here. But I've only
 * implemented SHA1 so this is it for now. */
const char ckmeth_sha1[] = { "SHA-1" };

/* List of options strings for gzip(1) allowed in the .zsync. This is 
 * security against someone specifying arbitrary commands. */
static const char* const gzip_safe_option[] = {
    "--best",
    "",
    "--rsync",
    "--rsync --best",
    "--best --no-name",
    "--no-name",
    "--rsync --no-name",
    "--rsync --best --no-name"
};
const int gzip_safe_options = sizeof(gzip_safe_option)/sizeof *gzip_safe_option;

/****************************************************************************
 *
 * zsync_state object and methods
 * This holds a single target file's details, and holds the state of the
 * in-progress local copy of that target that we are constructing (via a
 * contained rcksum_state object)
 *
 * Also holds all the other misc data from the .zsync file.
 */
struct zsync_state {
    struct rcksum_state *rs;    /* rsync algorithm state, with block checksums and
                                 * holding the in-progress local version of the target */
    off_t filelen;              /* Length of the target file */
    int blocks;                 /* Number of blocks in the target */
    size_t blocksize;           /* Blocksize */

    /* Checksum of the entire file, and checksum alg */
    char *checksum;
    const char *checksum_method;

    /* URLs to uncompressed versions of the target */
    char **url;
    int nurl;

    /* URLs to compressed versions of the target, and the zmap of that compressed version */
    struct zmap *zmap;
    char **zurl;
    int nzurl;

    char *cur_filename;         /* If we have taken the filename from rcksum, it is here */

    /* Hints for the output file, from the .zsync */
    char *filename;             /* The Filename: header */
    char *zfilename;            /* ditto Z-Filename: */

    char *gzopts;               /* If we're recompressing the download afterwards, these are the options to gzip(1) */
    char *gzhead;               /* And this is the header of the gzip file (for the mtime) */

    time_t mtime;               /* MTime: from the .zsync, or -1 */
};

static int zsync_read_blocksums(struct zsync_state *zs, FILE * f,
                                int rsum_bytes, int checksum_bytes,
                                int seq_matches);
static int zsync_sha1(struct zsync_state *zs, int fh);
static int zsync_recompress(struct zsync_state *zs);
static time_t parse_822(const char* ts);

/* char*[] = append_ptrlist(&num, &char[], "to add")
 * Crude data structure to store an ordered list of strings. This appends one
 * entry to the list. */
static char **append_ptrlist(int *n, char **p, char *a) {
    if (!a)
        return p;
    p = realloc(p, (*n + 1) * sizeof *p);
    if (!p) {
        LOGD("out of memory\n");
        exit(1);
    }
    p[*n] = a;
    (*n)++;
    return p;
}

/* Constructor */
struct zsync_state *zsync_begin(FILE * f) {
    /* Defaults for the checksum bytes and sequential matches properties of the
     * rcksum_state. These are the defaults from versions of zsync before these
     * were variable. */
    int checksum_bytes = 16, rsum_bytes = 4, seq_matches = 1;

    /* Field names that we can ignore if present and not
     * understood. This allows new headers to be added without breaking
     * backwards compatibility, and conversely to add headers that do break
     * backwards compat and have old clients give meaningful errors. */
    char *safelines = NULL;

    /* Allocate memory for the object */
    struct zsync_state *zs = calloc(sizeof *zs, 1);

    if (!zs)
        return NULL;

    /* Any non-zero defaults here. */
    zs->mtime = -1;

    for (;;) {
        char buf[1024];
        char *p = NULL;
        int l;

        if (fgets(buf, sizeof(buf), f) != NULL) {
            if (buf[0] == '\n')
                break;
            l = strlen(buf) - 1;
            while (l >= 0 && (buf[l] == '\n' || buf[l] == '\r' || buf[l] == ' '))
                buf[l--] = 0;

            p = strchr(buf, ':');
        }
        if (p && *(p + 1) == ' ') {
            *p++ = 0;
            p++;
            if (!strcmp(buf, "zsync")) {
                if (!strcmp(p, "0.0.4")) {
                    LOGD("This version of zsync is not compatible with zsync 0.0.4 streams.\n");
                    free(zs);
                    return NULL;
                }
            }
            else if (!strcmp(buf, "Min-Version")) {
                if (strcmp(p, VERSION) > 0) {
                    LOGD("control file indicates that zsync-%s or better is required\n", p);
                    free(zs);
                    return NULL;
                }
            }
            else if (!strcmp(buf, "Length")) {
                zs->filelen = atoll(p);
            }
            else if (!strcmp(buf, "Filename")) {
                zs->filename = strdup(p);
            }
            else if (!strcmp(buf, "Z-Filename")) {
                zs->zfilename = strdup(p);
            }
            else if (!strcmp(buf, "URL")) {
                zs->url = (char **)append_ptrlist(&(zs->nurl), zs->url, strdup(p));
            }
            else if (!strcmp(buf, "Z-URL")) {
                zs->zurl = (char **)append_ptrlist(&(zs->nzurl), zs->zurl, strdup(p));
            }
            else if (!strcmp(buf, "Blocksize")) {
                zs->blocksize = atol(p);
                if (zs->blocksize & (zs->blocksize - 1))
                {
                    LOGD("nonsensical blocksize %ld\n", zs->blocksize);
                    free(zs);
                    return NULL;
                }
            }
            else if (!strcmp(buf, "Hash-Lengths")) {
                if (sscanf
                    (p, "%d,%d,%d", &seq_matches, &rsum_bytes,
                     &checksum_bytes) != 3 || rsum_bytes < 1 || rsum_bytes > 4
                    || checksum_bytes < 3 || checksum_bytes > 16
                    || seq_matches > 2 || seq_matches < 1) {
                    LOGD("nonsensical hash lengths line %s\n", p);
                    free(zs);
                    return NULL;
                }
            }
            else if (zs->blocks && !strcmp(buf, "Z-Map2")) {
                int nzblocks;
                struct gzblock *zblock;

                nzblocks = atoi(p);
                if (nzblocks < 0) {
                    LOGD("bad Z-Map line\n");
                    free(zs);
                    return NULL;
                }

                zblock = malloc(nzblocks * sizeof *zblock);
                if (zblock) {
                    if (fread(zblock, sizeof *zblock, nzblocks, f) < nzblocks) {
                        LOGD("premature EOF after Z-Map\n");
                        free(zs);
                        return NULL;
                    }

                    zs->zmap = zmap_make(zblock, nzblocks);
                    free(zblock);
                }
            }
            else if (!strcmp(buf, ckmeth_sha1)) {
                if (strlen(p) != SHA_DIGEST_LENGTH * 2) {
                    LOGD("SHA-1 digest from control file is wrong length.\n");
                }
                else {
                    zs->checksum = strdup(p);
                    zs->checksum_method = ckmeth_sha1;
                }
            }
            else if (!strcmp(buf, "Safe")) {
                safelines = strdup(p);
            }
            else if (!strcmp(buf, "Recompress")) {
                zs->gzhead = strdup(p);
                if (zs->gzhead) {
                    int i;
                    char *q = strchr(zs->gzhead, ' ');
                    if (!q)
                        q = zs->gzhead + strlen(zs->gzhead);

                    if (*q)
                        *q++ = 0;
                    /* Whitelist for safe options for gzip command line */
                    for (i = 0; i < gzip_safe_options; i++)
                        if (!strcmp(q, gzip_safe_option[i])) {
                            zs->gzopts = strdup(q);
                            break;
                        }
                    if( !zs->gzopts ) {
                        LOGD("bad recompress options, rejected\n");
                        free(zs->gzhead);
                    }
                }
            }
            else if (!strcmp(buf, "MTime")) {
                zs->mtime = parse_822(p);
            }
            else if (!safelines || !strstr(safelines, buf)) {
                LOGD("unrecognised tag %s - you need a newer version of zsync.\n", buf);
                free(zs);
                return NULL;
            }
            if (zs->filelen && zs->blocksize)
                zs->blocks = (zs->filelen + zs->blocksize - 1) / zs->blocksize;
        }
        else {
            LOGD("Bad line - not a zsync file? \"%s\"\n", buf);
            free(zs);
            return NULL;
        }
    }
    if (!zs->filelen || !zs->blocksize) {
        LOGD("Not a zsync file (looked for Blocksize and Length lines)\n");
        free(zs);
        return NULL;
    }
    if (zsync_read_blocksums(zs, f, rsum_bytes, checksum_bytes, seq_matches) != 0) {
        free(zs);
        return NULL;
    }
    return zs;
}

/* zsync_read_blocksums(self, FILE*, rsum_bytes, checksum_bytes, seq_matches)
 * Called during construction only, this creates the rcksum_state that stores
 * the per-block checksums of the target file and holds the local working copy
 * of the in-progress target. And it populates the per-block checksums from the
 * given file handle, which must be reading from the .zsync at the start of the
 * checksums. 
 * rsum_bytes, checksum_bytes, seq_matches are settings for the checksums,
 * passed through to the rcksum_state. */
static int zsync_read_blocksums(struct zsync_state *zs, FILE * f,
                                int rsum_bytes, int checksum_bytes,
                                int seq_matches) {
    /* Make the rcksum_state first */
    if (!(zs->rs = rcksum_init(zs->blocks, zs->blocksize, rsum_bytes, checksum_bytes, seq_matches))) {
        return -1;
    }

    /* Now read in and store the checksums */
    for (zs_blockid id=0; id<zs->blocks; id++) {
        struct rsum r = { 0, 0 };
        unsigned char checksum[CHECKSUM_SIZE];

        /* Read in */
        if (fread(((char *)&r) + 4 - rsum_bytes, rsum_bytes, 1, f) < 1
            || fread((void *)&checksum, checksum_bytes, 1, f) < 1) {

            /* Error - free the rcksum_state and tell the caller to bail */
            LOGD("short read on control file; %s\n", strerror(ferror(f)));
            rcksum_end(zs->rs);
            return -1;
        }

        /* Convert to host endian and store */
        r.a = ntohs(r.a);
        r.b = ntohs(r.b);
        rcksum_add_target_block(zs->rs, id, r, checksum);
    }
    return 0;
}

/* parse_822(buf[])
 * Parse an RFC822 date string. Returns a time_t, or -1 on failure. 
 * E.g. Tue, 25 Jul 2006 20:02:17 +0000
 */
static time_t parse_822(const char* ts) {
    struct tm t;

    if (strptime(ts, "%a, %d %b %Y %H:%M:%S %z", &t) == NULL
        && strptime(ts, "%d %b %Y %H:%M:%S %z", &t) == NULL) {
        return -1;
    }
    return mktime(&t);
}

/* zsync_hint_decompress(self)
 * Returns true if we think we'll be able to download compressed data to get
 * the needed data to complete the target file */
int zsync_hint_decompress(const struct zsync_state *zs) {
    return (zs->nzurl > 0 ? 1 : 0);
}

/* zsync_blocksize(self)
 * Returns the blocksize used by zsync on this target. */
int zsync_blocksize(const struct zsync_state *zs) {
    return zs->blocksize;
}

/* char* = zsync_filename(self)
 * Returns the suggested filename to be used for the final result of this
 * zsync.  Malloced string to be freed by the caller. */
char *zsync_filename(const struct zsync_state *zs) {
    return strdup(zs->gzhead && zs->zfilename ? zs->zfilename : zs->filename);
}

/* time_t = zsync_mtime(self)
 * Returns the mtime on the original copy of the target; for the client program
 * to set the mtime of the local file to match, if it so chooses.
 * Or -1 if no mtime specified in the .zsync */
time_t zsync_mtime(const struct zsync_state *zs) {
    return zs->mtime;
}

/* zsync_status(self)
 * Returns  0 if we have no data in the target file yet.
 *          1 if we have some but not all
 *          2 or more if we have all.
 * The caller should not rely on exact values 2+; just test >= 2. Values >2 may
 * be used in later versions of libzsync. */
int zsync_status(const struct zsync_state *zs) {
    int todo = rcksum_blocks_todo(zs->rs);

    if (todo == zs->blocks)
        return 0;
    if (todo > 0)
        return 1;
    return 2;                   /* TODO: more? */
}

/* zsync_progress(self, &got, &total)
 * Writes the number of bytes got, and the total to get, into the long longs.
 */
void zsync_progress(const struct zsync_state *zs, long long *got,
                    long long *total) {

    if (got) {
        int todo = zs->blocks - rcksum_blocks_todo(zs->rs);
        *got = todo * zs->blocksize;
    }
    if (total)
        *total = zs->blocks * zs->blocksize;
}

/* zsync_get_urls(self, &num, &type)
 * Returns a (pointer to an) array of URLs (returning the number of them in
 * num) that are remote available copies of the target file (according to the
 * .zsync).
 * Note that these URLs could be for encoded versions of the target; a 'type'
 * is returned in *type which tells libzsync in later calls what version of the
 * target is being retrieved. */
const char *const *zsync_get_urls(struct zsync_state *zs, int *n, int *t) {
    if (zs->zmap && zs->nzurl) {
        *n = zs->nzurl;
        *t = 1;
        return (const char *const *)*zs->zurl;
    }
    else {
        *n = zs->nurl;
        *t = 0;
        return (const char *const *)*zs->url;
    }
}

/* zsync_needed_byte_ranges(self, &num, type)
 * Returns an array of offsets (2*num of them) for the start and end of num
 * byte ranges in the given type of version of the target (type as returned by
 * a zsync_get_urls call), such that retrieving all these byte ranges would be
 * sufficient to obtain a complete copy of the target file.
 */
off_t *zsync_needed_byte_ranges(struct zsync_state * zs, int *num, int type) {
    int nrange;
    off_t *byterange;
    int i;

    /* Request all needed block ranges */
    zs_blockid *blrange = rcksum_needed_block_ranges(zs->rs, &nrange, 0, 0x7fffffff);
    if (!blrange)
        return NULL;

    /* Allocate space for byte ranges */
    byterange = malloc(2 * nrange * sizeof *byterange);
    if (!byterange) {
        free(blrange);
        return NULL;
    }

    /* Now convert blocks to bytes.
     * Note: Must cast one operand to off_t as both blocksize and blrange[x]
     * are int's whereas the product must be a file offfset. Needed so we don't
     * truncate file offsets to 32bits on 32bit platforms. */
    for (i = 0; i < nrange; i++) {
        byterange[2 * i] = blrange[2 * i] * (off_t)zs->blocksize;
        byterange[2 * i + 1] = blrange[2 * i + 1] * (off_t)zs->blocksize - 1;
    }
    free(blrange);      /* And release the blocks, we're done with them */

    switch (type) {
    case 0:
        *num = nrange;
        return byterange;
    case 1:
        {   /* Convert ranges in the uncompressed data to ranges in the compressed data */
            off_t *zbyterange =
                zmap_to_compressed_ranges(zs->zmap, byterange, nrange, &nrange);

            /* Store the number of compressed ranges and return them, freeing
             * the uncompressed ones now we've used them. */
            if (zbyterange) {
                *num = nrange;
            }
            free(byterange);
            return zbyterange;
        }
    default:
        free(byterange);
        return NULL;
    }
}

/* zsync_submit_source_file(self, FILE*, progress)
 * Read the given stream, applying the rsync rolling checksum algorithm to
 * identify any blocks of data in common with the target file. Blocks found are
 * written to our local copy of the target in progress. Progress reports if
 * progress != 0  */
int zsync_submit_source_file(struct zsync_state *zs, FILE * f, int progress)
{
    return rcksum_submit_source_file(zs->rs, f, progress);
}

char *zsync_cur_filename(struct zsync_state *zs)
{
    if (!zs->cur_filename)
        zs->cur_filename = rcksum_filename(zs->rs);

    return zs->cur_filename;
}

/* zsync_rename_file(self, filename)
 * Tell libzsync to move the local copy of the target (or under construction
 * target) to the given filename. */
int zsync_rename_file(struct zsync_state *zs, const char *f)
{
    char *rf = zsync_cur_filename(zs);
    int x = rename(rf, f);

    if (!x)
    {
        free(rf);
        zs->cur_filename = strdup(f);
    }
    else
        perror("rename");

    return x;
}

/* int hexdigit(char)
 * Maps a character to 0..15 as a hex digit (or 0 if not valid hex digit)
 */
static inline int hexdigit(char c)
{
    return (isdigit(c) ? (c - '0') : isupper(c) ? (0xa + (c - 'A')) : islower(c)
            ? (0xa + (c - 'a')) : 0);
}

/* zsync_complete(self)
 * Finish a zsync download. Should be called once all blocks have been
 * retrieved successfully. This returns 0 if the file passes the final
 * whole-file checksum and if any recompression requested by the .zsync file is
 * done.
 * Returns -1 on error (and prints the error to stderr)
 *          0 if successful but no checksum verified
 *          1 if successful including checksum verified
 */
int zsync_complete(struct zsync_state *zs) {
    int rc = 0;

    /* We've finished with the rsync algorithm. Take over the local copy from
     * librcksum and free our rcksum state. */
    int fh = rcksum_filehandle(zs->rs);
    zsync_cur_filename(zs);
    rcksum_end(zs->rs);
    zs->rs = NULL;

    /* Truncate the file to the exact length (to remove any trailing NULs from
     * the last block); return to the start of the file ready to verify. */
    if (ftruncate(fh, zs->filelen) != 0) {
        perror("ftruncate");
        rc = -1;
    }
    if (lseek(fh, 0, SEEK_SET) != 0) {
        perror("lseek");
        rc = -1;
    }

    /* Do checksum check */
    if (rc == 0 && zs->checksum && !strcmp(zs->checksum_method, ckmeth_sha1)) {
        rc = zsync_sha1(zs, fh);
    }
    close(fh);
    sync();

    /* Do any requested recompression */
    if (rc >= 0 && zs->gzhead && zs->gzopts) {
        if (zsync_recompress(zs) != 0) {
            return -1;
        }
    }
    return rc;
}

/* zsync_sha1(self, filedesc)
 * Given the currently-open-and-at-start-of-file complete local copy of the
 * target, read it and compare the SHA1 checksum with the one from the .zsync.
 * Returns -1 or 1 as per zsync_complete.
 */
static int zsync_sha1(struct zsync_state *zs, int fh) {
    SHA_CTX shactx;

    {                           /* Do SHA1 of file contents */
        unsigned char buf[4096];
        int rc;

        SHA1_Init(&shactx);
        while (0 < (rc = read(fh, buf, sizeof buf))) {
            SHA1_Update(&shactx, buf, rc);
        }
        if (rc < 0) {
            perror("read");
            return -1;
        }
    }

    {                           /* And compare result of the SHA1 with the one from the .zsync */
        unsigned char digest[SHA_DIGEST_LENGTH];
        int i;

        SHA1_Final(digest, &shactx);

        for (i = 0; i < SHA_DIGEST_LENGTH; i++) {
            int j;
            sscanf(&(zs->checksum[2 * i]), "%2x", &j);
            if (j != digest[i]) {
                return -1;
            }
        }
        return 1; /* Checksum verified okay */
    }
}

/* zsync_recompress(self)
 * Called when we have a complete local copy of the uncompressed data, to
 * perform compression requested in the .zsync.
 *
 * Shells out to the standard system gzip(1). Replaces the gzip file header
 * with the one supplied in the .zsync; this means we should get an identical
 * compressed file output to the original compressed file on the source system
 * (to allow the user to verify a checksum on the compressed file, or just
 * because the user is picky and wants their compressed file to match the
 * original).
 *
 * Returns 0 on success, -1 on error (which is reported on stderr). */
static int zsync_recompress(struct zsync_state *zs) {
    /* Recompression. This is a fugly mess, calling gzip on the temporary file with options
     *  read out of the .zsync, reading its output and replacing the gzip header. Ugh. */
    FILE *g;
    FILE *zout;
    int rc = 0;

    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "gzip -n %s < ", zs->gzopts);

    {   /* Add input filename, shell-escaped, to the command line */
        int i = 0;
        size_t j = strlen(cmd);
        char c;

        while ((c = zs->cur_filename[i++]) != 0 && j < sizeof(cmd) - 2) {
            if (!isalnum(c))
                cmd[j++] = '\\';
            cmd[j++] = c;
        }
        cmd[j] = 0;
    }

    /* Read gzipped version of the data via pipe from gzip; write it to our new
     * output file, except that we replace the gzip header with our own from
     * the .zsync file. */
    g = popen(cmd, "r");
    if (g) {
        char zoname[1024];

        snprintf(zoname, sizeof(zoname), "%s.gz", zs->cur_filename);
        zout = fopen(zoname, "w");

        if (zout) {
            char *p = zs->gzhead;
            int skip = 1;

            while (p[0] && p[1]) {
                if (fputc((hexdigit(p[0]) << 4) + hexdigit(p[1]), zout) == EOF) {
                    perror("putc");
                    rc = -1;
                }
                p += 2;
            }
            while (!feof(g)) {
                char buf[1024];
                int r;
                const char *p = buf;

                if ((r = fread(buf, 1, sizeof(buf), g)) < 0) {
                    perror("fread");
                    rc = -1;
                    goto leave_it;
                }
                if (skip) {
                    p = skip_zhead(buf);
                    skip = 0;
                }
                if (fwrite(p, 1, r - (p - buf), zout) != r - (p - buf)) {
                    perror("fwrite");
                    rc = -1;
                    goto leave_it;
                }
            }

          leave_it:
            if (fclose(zout) != 0) {
                perror("close");
                rc = -1;
            }
        }
        if (fclose(g) != 0) {
            perror("close");
            rc = -1;
        }

        /* Free our old filename and replace with the new one */
        unlink(zs->cur_filename);
        free(zs->cur_filename);
        zs->cur_filename = strdup(zoname);
    }
    else {
        LOGD("problem with gzip, unable to compress.\n");
    }
    return rc;
}

/* Destructor */
char *zsync_end(struct zsync_state *zs) {
    int i;
    char *f = zsync_cur_filename(zs);

    /* Free rcksum object and zmap */
    if (zs->rs)
        rcksum_end(zs->rs);
    if (zs->zmap)
        zmap_free(zs->zmap);

    /* Clear download URLs */
    for (i = 0; i < zs->nurl; i++)
        free(zs->url[i]);
    for (i = 0; i < zs->nzurl; i++)
        free(zs->zurl[i]);

    /* And the rest. */
    free(zs->url);
    free(zs->zurl);
    free(zs->checksum);
    free(zs->filename);
    free(zs->zfilename);
    free(zs);
    return f;
}

#if SUPPORT_GZIP_STREAM
/* Next come the methods for accepting data received from the remote copies of
 * the target and incomporating them into the local copy under construction. */

/* zsync_configure_zstream_for_zdata(self, &z_stream_s, zoffset, &outoffset)
 * Rewrites the state in the given zlib stream object to be ready to decompress
 * data from the compressed version of this zsync stream at the given offset in
 * the compressed file. Returns the offset in the uncompressed stream that this
 * corresponds to in the 4th parameter. 
 */
void zsync_configure_zstream_for_zdata(const struct zsync_state *zs,
                                       struct z_stream_s *zstrm,
                                       long zoffset, long long *poutoffset) {
    configure_zstream_for_zdata(zs->zmap, zstrm, zoffset, poutoffset);
    {                           /* Load in prev 32k sliding window for backreferences */
        long long pos = *poutoffset;
        int lookback = (pos > 32768) ? 32768 : pos;

        /* Read in 32k of leading uncompressed context - needed because the deflate
         * compression method includes back-references to previously-seen strings. */
        unsigned char wbuf[32768];
        rcksum_read_known_data(zs->rs, wbuf, pos - lookback, lookback);

        /* Fake an output buffer of 32k filled with data to zlib */
        zstrm->next_out = wbuf + lookback;
        zstrm->avail_out = 0;
        updatewindow(zstrm, lookback);
    }
}
#endif

/* zsync_submit_data(self, buf[], offset, blocks)
 * Passes data retrieved from the remote copy of
 * the target file to libzsync, to be written into our local copy. The data is
 * the given number of blocks at the given offset (must be block-aligned), data
 * in buf[].  */
static int zsync_submit_data(struct zsync_state *zs, const unsigned char *buf, off_t offset, int blocks)
{
    zs_blockid blstart = offset / zs->blocksize;
    zs_blockid blend = blstart + blocks - 1;

    return rcksum_submit_blocks(zs->rs, buf, blstart, blend);
}

/****************************************************************************
 *
 * zsync_receiver object definition and methods.
 * Stores the state for a currently-running download of blocks from a
 * particular URL or version of a file to complete a file using zsync.
 *
 * This is mostly a wrapper for the zsync_state which keeps various additional
 * state needed per-download: in particular the zlib stream object to
 * decompress the incoming data if this is a URL of a compressed version of the
 * target file.
 */
struct zsync_receiver
{
    struct zsync_state *zs;     /* The zsync_state that we are downloading for */
    struct z_stream_s strm;     /* Decompression object */
    int url_type;               /* Compressed or not */
    unsigned char *outbuf;      /* Working buffer to keep incomplete blocks of data */
    off_t outoffset;            /* and the position in that buffer */
};

/* Constructor */
struct zsync_receiver *zsync_begin_receive(struct zsync_state *zs, int url_type)
{
    struct zsync_receiver *zr = malloc(sizeof(struct zsync_receiver));

    if (!zr)
        return NULL;
    zr->zs = zs;

    zr->outbuf = malloc(zs->blocksize);
    if (!zr->outbuf)
    {
        free(zr);
        return NULL;
    }

    /* Set up new inflate object */
    zr->strm.zalloc = Z_NULL;
    zr->strm.zfree = Z_NULL;
    zr->strm.opaque = NULL;
    zr->strm.total_in = 0;

    zr->url_type = url_type;
    zr->outoffset = 0;

    return zr;
}

/* zsync_receive_data_uncompressed(self, buf[], offset, buflen)
 * Adds the data in buf (buflen bytes) to this file at the given offset.
 * Returns 0 unless there's an error (e.g. the submitted data doesn't match the
 * expected checksum for the corresponding blocks)
 */
static int zsync_receive_data_uncompressed(struct zsync_receiver *zr,
                                           const unsigned char *buf,
                                           off_t offset, size_t len) {
    int ret = 0;
    size_t blocksize = zr->zs->blocksize;

    if (0 != (offset % blocksize)) {
        size_t x = len;

        if (x > blocksize - (offset % blocksize))
            x = blocksize - (offset % blocksize);

        if (zr->outoffset == offset) {
            /* Half-way through a block, so let's try and complete it */
            if (len)
                memcpy(zr->outbuf + offset % blocksize, buf, x);
            else {
                // Pad with 0s to length.
                memset(zr->outbuf + offset % blocksize, 0, len = x = blocksize - (offset % blocksize));
            }

            if ((x + offset) % blocksize == 0)
                if (zsync_submit_data(zr->zs, zr->outbuf, zr->outoffset + x - blocksize, 1))
                    ret = 1;
        }
        buf += x;
        len -= x;
        offset += x;
    }

    /* Now we are block-aligned */
    if (len >= blocksize) {
        int w = len / blocksize;

        if (zsync_submit_data(zr->zs, buf, offset, w))
            ret = 1;
        else
            ret = 0;

        w *= blocksize;
        buf += w;
        len -= w;
        offset += w;

    }
    /* Store incomplete block */
    if (len) {
        memcpy(zr->outbuf, buf, len);
        // try pad data with zero and submit
        memset(zr->outbuf + len, 0, blocksize-len);
        if (zsync_submit_data(zr->zs, zr->outbuf, offset, 1))
            offset += len;          /* not needed: buf += len; len -= len; */
        else
            offset += blocksize;
    }

    zr->outoffset = offset;
    return ret;
}

#if SUPPORT_GZIP_STREAM
/* zsync_receive_data_compressed(self, buf[], offset, buflen)
 * Passes data received corresponding to the compressed version of this file at
 * the given offset; data in buf, buflen bytes.
 * Returns 0 unless there's an error (e.g. the submitted data doesn't match the
 * expected checksum for the corresponding blocks)
 */
static int zsync_receive_data_compressed(struct zsync_receiver *zr,
                              const unsigned char *buf, off_t offset,
                              size_t len) {
    int ret = 0;
    int eoz = 0;
    size_t blocksize = zr->zs->blocksize;

    if (!len)
        return 0;

    /* Now set up for the downloaded block */
    zr->strm.next_in = buf;
    zr->strm.avail_in = len;

#if SUPPORT_GZIP_STREAM
    if (zr->strm.total_in == 0 || offset != zr->strm.total_in)
    {
        zsync_configure_zstream_for_zdata(zr->zs, &(zr->strm), offset,
                                          &(zr->outoffset));

        /* On first iteration, we might be reading an incomplete block from zsync's point of view. Limit avail_out so we can stop after doing that and realign with the buffer. */
        zr->strm.avail_out = blocksize - (zr->outoffset % blocksize);
        zr->strm.next_out = zr->outbuf;
    }
    else
#endif
    {
        if (zr->outoffset == -1) {
            CCLog("data didn't align with block boundary in compressed stream\n");
            return 1;
        }
        zr->strm.next_in = buf;
        zr->strm.avail_in = len;
    }

    while (zr->strm.avail_in && !eoz) {
        int rc;

        /* Read in up to the next block (in the libzsync sense on the output stream) boundary */

        rc = inflate(&(zr->strm), Z_SYNC_FLUSH);
        switch (rc) {
        case Z_STREAM_END:
            eoz = 1;
        case Z_BUF_ERROR:
        case Z_OK:
            if (zr->strm.avail_out == 0 || eoz) {
                /* If this was at the start of a block, try submitting it */
                if (!(zr->outoffset % blocksize)) {
                    int rc;

                    if (zr->strm.avail_out)
                        memset(zr->strm.next_out, 0, zr->strm.avail_out);
                    rc = zsync_submit_data(zr->zs, zr->outbuf,
                                           zr->outoffset, 1);
                    if (!zr->strm.avail_out)
                        ret |= rc;
                    zr->outoffset += blocksize;
                }
                else {
                    /* We were reading a block fragment; update outoffset, and we are now block-aligned. */
                    zr->outoffset += (zr->strm.next_out - zr->outbuf);
                }
                zr->strm.avail_out = blocksize;
                zr->strm.next_out = zr->outbuf;
            }
            break;
        default:
            CCLog("zlib error: %s (%d)\n", zr->strm.msg, rc);
            eoz = 1;
            ret = -1;
            break;
        }
    }
    return ret;
}
#endif

/* zsync_receive_data(self, buf[], offset, buflen)
 * Passes data received from the source URL at the given offset; 
 * data is buflen bytes in buf[].
 * Returns 0 unless there's an error (e.g. the submitted data doesn't match the
 * expected checksum for the corresponding blocks)
 */
int zsync_receive_data(struct zsync_receiver *zr, const unsigned char *buf, off_t offset, size_t len)
{
#if SUPPORT_GZIP_STREAM
    if (zr->url_type == 1) {
        return zsync_receive_data_compressed(zr, buf, offset, len);
    }
    else
#endif
    {
        return zsync_receive_data_uncompressed(zr, buf, offset, len);
    }
}

/* Destructor */
void zsync_end_receive(struct zsync_receiver *zr) {
    if (zr->strm.total_in > 0) {
        inflateEnd(&(zr->strm));
    }
    free(zr->outbuf);
    free(zr);
}
