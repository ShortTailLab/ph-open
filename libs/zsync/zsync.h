/*
 *   zsync - client side rsync over http
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

#ifdef __cplusplus
extern "C" {
#endif


struct zsync_state;

/* zsync_begin - load a zsync file and return data structure to use for the rest of the process.
 */
struct zsync_state* zsync_begin(FILE* cf);

/* zsync_hint_decompress - if it returns non-zero, this suggests that 
 *  compressed seed files should be decompressed */
int zsync_hint_decompress(const struct zsync_state*);

/* zsync_filename - return the suggested filename from the .zsync file */
char* zsync_filename(const struct zsync_state*);
/* zsync_mtime - return the suggested mtime from the .zsync file */
time_t zsync_mtime(const struct zsync_state*);

/* zsync_rename_file - renames the temporary file used by zsync to the given name.
 * You don't "own" the filename until you zsync_end, but you can use this to give zsync a more 
 * appropriate intermediate filename (in case the user ctrl-c's). 
 * This is purely a hint; zsync could ignore it. Returns 0 if successful. */
int zsync_rename_file(struct zsync_state* zs, const char* f);

/* zsync_status - returns the current state:
 * 0 - no relevant local data found yet.
 * 1 - some data present
 * 2+ - all data downloaded (higher values may be added later to indicate completion
 *      of checksumming and file handle states)
 */

int zsync_status(const struct zsync_state* zs);

/* zsync_progress - returns bytes of the file known so far in *got,
 * and the total (roughly, the file length) in *total */
void zsync_progress(const struct zsync_state* zs, long long* got, long long* total);

/* zsync_submit_source_file - submit local file data to zsync
 */
int zsync_submit_source_file(struct zsync_state* zs, FILE* f, int progress);

/* zsync_get_url - returns a URL from which to get needed data.
 * Returns NULL on failure, or a array of pointers to URLs.
 * Returns the size of the array in *n,
 * and the url type (to pass to needed_byte_ranges & begin_receive)
 * (the URL pointers are still referenced by the library, and are valid only until zsync_end).
 */

const char * const * zsync_get_urls(struct zsync_state* zs, int* n, int* t);

/* zsync_needed_byte_ranges - get the byte ranges needed from a URL.
 * Returns the number of ranges in *num, and a malloc'd array (to be freed 
 * by the caller) of 2*(*num) off_t's which are the starts and ends 
 * of byte ranges.
 */

off_t* zsync_needed_byte_ranges(struct zsync_state* zs, int* num, int type);

/* zsync_complete - set file length and verify checksum if available
 * Returns -1 for failure, 1 for success, 0 for unable to verify (e.g. no checksum in the .zsync) */
int zsync_complete(struct zsync_state* zs);

/* Clean up and free all resources. The pointer is freed by this call.
 * Returns a strdup()d pointer to the name of the file resulting from the process. */
char* zsync_end(struct zsync_state* zs);

/* And functions for receiving data on the network */
struct zsync_receiver;

/* Begin and end receiving from a particular URL.
 * Note that the zsync_receiver stores a reference to the zsync_state, 
 *  and libzsync does not do reference counting, so it is the caller's 
 *  responsibility not to do a zsync_end without doing a zsync_end_receive 
 *  first.
 * The url_type is as in the value returned by zsync_get_url.
 */
struct zsync_receiver* zsync_begin_receive(struct zsync_state*zs, int url_type);
void zsync_end_receive(struct zsync_receiver* zr);

/* Supply data buf of length len received corresponding to offset offset from the URL.
 * Returns 0 for success; if not, you should not submit more data. */
int zsync_receive_data(struct zsync_receiver* zr, const unsigned char* buf, off_t offset, size_t len);

#ifdef __cplusplus
}
#endif
