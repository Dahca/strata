/* GLIB - Library of useful routines for C programming
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/.
 */

#ifndef __PMEM_NVM_IDX_G_HASH_MOD_H__
#define __PMEM_NVM_IDX_G_HASH_MOD_H__ 1

#ifdef __cplusplus
extern "C" {
#define _Static_assert static_assert
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <libpmem.h>
#include <stdatomic.h>

#include "fs.h"
#include "storage/storage.h"
#include "global/global.h"
#include "shared.h"

// from common
#include "common/common.h"

// local includes
#include "hash_functions.h"

#define SIMPLE_ENTRIES


//#undef SIMPLE_ENTRIES

// For the big hash table, mapping (inode, lblk) -> single block
//struct hash_index_entry{
// #ifdef SIMPLE_ENTRIES
//    paddr_t key;
//    paddr_t value;
// #else
//     paddr_t  key;
//     uint8_t  index; // Go backwards to modify size on truncate
//     uint8_t  size;
//     uint16_t value_hi16;
//     uint32_t value_low32;
// #endif
//};
// _Static_assert(16 % sizeof(hash_ent_t) == 0, "Entries cross block boundary!");

#ifdef SIMPLE_ENTRIES
#define HASH_ENT_IS_TOMBSTONE(x) (x == ((paddr_t)~0) - 1)
#define HASH_ENT_IS_EMPTY(x) (x == (paddr_t)~0)
#define HASH_ENT_IS_VALID(x) (!HASH_ENT_IS_EMPTY(x) && !HASH_ENT_IS_TOMBSTONE(x))

#define HASH_ENT_SET_TOMBSTONE(x) (x = ((paddr_t)~0) - 1)
#define HASH_ENT_SET_EMPTY(x) (x = (paddr_t)~0)
#define HASH_ENT_SET_VAL(x,v) (x = v)
#else
#define HASH_ENT_VAL(x) (((paddr_t)(x).value_hi16 << 32) | ((paddr_t)(x).value_low32))
#define HASH_ENT_IS_TOMBSTONE(x) ((x).value_hi16 == (uint16_t)~0 && \
                                  (x).value_low32 == (uint32_t)~0)
#define HASH_ENT_IS_EMPTY(x) ((x).value_hi16 == 0 && (x).value_low32 == 0)
#define HASH_ENT_IS_VALID(x) (!HASH_ENT_IS_EMPTY(x) && !HASH_ENT_IS_TOMBSTONE(x))

#define HASH_ENT_SET_TOMBSTONE(x) do {(x).value_hi16 = ~0; \
                                      (x).value_low32 = ~0;} while(0)
#define HASH_ENT_SET_EMPTY(x) do {(x).value_hi16 = 0; (x).value_low32 = 0;} while(0)
#define HASH_ENT_SET_VAL(x,v) do {(x).value_hi16 = (uint16_t)(v >> 32); \
                                  (x).value_low32 = (uint32_t)(v);} while(0)
#endif

//#define RANGE_SIZE (1 << 5) // 32
#define RANGE_SIZE (1 << 9) // 512 -- 2MB
#define RANGE_BITS (RANGE_SIZE - 1)
#define RANGE_MASK (~RANGE_BITS)
#define RANGE_KEY(i, l) ( (((uint64_t)(i)) << 32) | ((l) & RANGE_MASK))

#define HASH_IS_REAL(h_) ((h_) >= 2)

//#define BLK_IDX(ht, x) (x % (ht->blksz / sizeof(hash_ent_t)))
//#define BLK_NUM(ht, x) (x / (ht->blksz / sizeof(hash_ent_t)))

/*
#define HASHCACHE

This is the hash table meta-data that is persisted to NVRAM, that we may read
it and know everything we need to know in order to reconstruct it in memory.
typedef struct device_hashtable_metadata {
  // Metadata for the in-memory state.
  uint32_t size;
  uint32_t mod;
  uint32_t mask;
  uint32_t nnodes;
  uint32_t noccupied;
  // Metadata about the on-disk state.
  size_t  blksz;
  paddr_t nvram_size;
  paddr_t range_size;
  paddr_t data_start;
} dev_hash_metadata_t;

typedef struct hashtable_stats {
    uint64_t n_lookups;
    uint64_t n_min_ents_per_lookup;
    uint64_t n_max_ents_per_lookup;
    uint64_t n_ents;
    STAT_FIELD(loop_time);
} hash_stats_t;

static void print_hashtable_stats(hash_stats_t *s) {
    printf("hashtable stats: \n");
    printf("\tAvg. collisions: %.2f\n", (double)s->n_ents / (double)s->n_lookups);
    printf("\tMin. collisions: %llu\n", s->n_min_ents_per_lookup);
    printf("\tMax. collisions: %llu\n", s->n_max_ents_per_lookup);
    PFIELD(s, loop_time);
}
*/
//need to include global.h, shared.h, fs.h
typedef struct pmem_nvm_hashtable_volatile_metadata {
	paddr_t *entries;
	hash_func_t hash_func;
} pmem_nvm_hash_vol_t;

typedef struct pmem_nvm_hashtable_index {
    int             is_pmem;
    int             valid;
    int 	    meta_size;
    int             size;
    int             mod;
    unsigned        mask;
    int             nnodes;
    int             noccupied;  /* nnodes + tombstones */

    paddr_t entries_blk; /* persists */
    //paddr_t *entries; /* run-time */
    uint64_t num_entries;
    //paddr_t         metadata;
    //paddr_t         data;
    //char           *data_ptr;
    //paddr_t         nvram_size;
    //size_t          blksz;
    //size_t          range_size;
    //size_t          nblocks;
    //hash_func_t      hash_func;
    //int              ref_count;

    // callbacks
    //callback_fns_t *idx_callbacks;
    //mem_man_fns_t  *idx_mem_man;

    // device infomation
    //device_info_t  *devinfo;

    // concurrency
    //pthread_rwlock_t *locks;
    //pthread_mutex_t *metalock;

    // -- CACHING
    //bool do_lock;
    //bool do_cache;
    //pthread_rwlock_t *cache_lock;
    //int8_t* cache_state;
    // array of entries
    //hash_ent_t *cache;

    // -- STATS
    //bool enable_stats;
    //hash_stats_t stats;
} pmem_nvm_hash_idx_t;

extern uint8_t *dax_addr[];
extern pmem_nvm_hash_idx_t *pmem_ht;



void
pmem_nvm_hash_table_new (struct superblock sblk,
                    hash_func_t       hash_func,
                    size_t            max_entries
                    //size_t            block_size,
                    //size_t            range_size,
                    //paddr_t           metadata_location,
                    //const idx_spec_t *idx_spec
                    );

void
pmem_nvm_hash_table_close ();
//void nvm_hash_table_destroy(nvm_hash_idx_t     *hash_table);

int pmem_nvm_hash_table_insert(paddr_t         key,
                          paddr_t         *index//,
                          //size_t          index,
                          //size_t          range
                          );

// Used for truncate
int pmem_nvm_hash_table_update(paddr_t         key,
                          size_t          new_range);

int pmem_nvm_hash_table_remove(paddr_t         key,
                          paddr_t        *value/*,
                          size_t         *nprevious,
                          size_t         *nblocks*/);

int pmem_nvm_hash_table_lookup(paddr_t key,
    paddr_t *val/*, paddr_t *size, bool force*/);

int pmem_nvm_hash_table_contains(paddr_t key);

unsigned pmem_nvm_hash_table_size();


extern uint64_t reads;
extern uint64_t writes;
extern uint64_t blocks;


/*
 * Convenience wrapper for when you need to look up the single value within
 * the block and nothing else. Index is offset from start (bytes).
 */
/*#if 0
static void
nvm_read_entry(nvm_hash_idx_t *ht, paddr_t idx, hash_ent_t **ret, bool force) {

    paddr_t block  = ht->data + BLK_NUM(ht, idx);
    off_t   offset = BLK_IDX(ht, idx) * sizeof(**ret);

    if (!ht->do_cache) {
#if 0
        ssize_t err = CB(ht, cb_get_addr, block, offset, (char**)ret);
        if_then_panic(err, "Could not get device address!");
#else
        *ret = (hash_ent_t*)(ht->data_ptr + (BLK_NUM(ht, idx) * ht->blksz) + offset);
#endif
                        
    } else if (ht->do_cache && ht->cache_state[idx] < 0) {
        ssize_t err = CB(ht, cb_read, 
                         block, offset, sizeof(*ht->cache), (char*)&(ht->cache[idx]));

        if_then_panic(sizeof(*ht->cache) != err, "Did not read enough bytes!");

        ht->cache_state[idx] = 0;

        *ret = &(ht->cache[idx]);
    }


    reads++;
}
#else
#define nvm_read_entry(ht, idx, ret, force) \
    do { *ret = (hash_ent_t*)(ht->data_ptr + (BLK_NUM(ht, idx) * ht->blksz) + (BLK_IDX(ht, idx) * sizeof(**ret))); } while(0)
#endif
*/

/*
 * Read the hashtable metadata from disk. If the size is zero, then we need to
 * allocate the table. Otherwise, the structures have already been
 * pre-allocated.
 *
 * Returns 1 on success, 0 on failure.
 */

/*
static int
nvm_read_metadata(nvm_hash_idx_t *ht) {

    dev_hash_metadata_t metadata;
    ssize_t err = CB(ht, cb_read, ht->metadata, 0,
                                         sizeof(metadata), (char*)&metadata);

    if_then_panic(err != sizeof(metadata), "Could not read metadata!");

    // now check the actual metadata
    if (metadata.nvram_size == 0) {
        return 0;
    }

    assert(ht->nvram_size == metadata.nvram_size);
    assert(ht->range_size == metadata.range_size);
    // reconsititute the rest of the httable from
    ht->size = metadata.size;
    ht->blksz = metadata.blksz;
    ht->mod = metadata.mod;
    ht->mask = metadata.mask;
    ht->nnodes = metadata.nnodes;
    ht->noccupied = metadata.noccupied;
    ht->data = metadata.data_start;

    return 1;
}
*/


/*
static int
nvm_write_metadata(nvm_hash_idx_t *ht) {
    dev_hash_metadata_t metadata;

    // reconsititute the rest of the httable from
    metadata.nvram_size = ht->nvram_size;
    metadata.blksz = ht->blksz;
    metadata.size = ht->size;
    metadata.range_size = ht->range_size;
    metadata.mod = ht->mod;
    metadata.mask = ht->mask;
    metadata.nnodes = ht->nnodes;
    metadata.noccupied = ht->noccupied;
    metadata.data_start = ht->data;

    ssize_t err = CB(ht, cb_write, ht->metadata, 0,
                                          sizeof(metadata), (char*)&metadata);

    if_then_panic(err != sizeof(metadata), "Could not write metadata!");

    return 1;
}
*/

/*
 * Update a single slot in NVRAM.
 * Used to insert or update a key -- since we'll never need to modify keys
 * en-masse, this will be fine.
 *
 * start: block address of range.
 * index: byte index into range.
 */
/* static inline void
// nvm_update(nvm_hash_idx_t *ht, paddr_t idx) {

    if (ht->do_cache) {
#if 0
        paddr_t paddr = ht->data + BLK_NUM(ht, idx);
        ssize_t size  = sizeof(*ht->cache);
        off_t offset  = BLK_IDX(ht, idx) * size;

        ssize_t ret = CB(ht, cb_write, 
                         paddr, offset, size, (char*)&(ht->cache[idx]));

        if_then_panic(ret != size, "did not write full entry!");
#endif
        ht->cache_state[idx] = 1;
    }
    // This is a no-op for non-cached, as it reads directly from the device.

}
*/


/*
static inline int nvm_invalidate(nvm_hash_idx_t *ht) {
    if (unlikely(!ht->do_cache)) return 0;

    pthread_rwlock_wrlock(ht->cache_lock);
    memset(ht->cache_state, -1, ht->nvram_size);
    pthread_rwlock_unlock(ht->cache_lock);
    return 0;
} 
*/

/*
 * The more interesting function. Find ranges and write them all back at once.
 */
// static inline int nvm_persist(nvm_hash_idx_t *ht) {
//     if (unlikely(!ht->do_cache)) return 0;

//     pthread_rwlock_wrlock(ht->cache_lock);
//     for (paddr_t idx = 0; idx < ht->nvram_size; ++idx) {
//         if (ht->cache_state[idx] > 0) {
//             paddr_t end = idx;
//             while (ht->cache_state[end] > 0) {
//                 ht->cache_state[end] = 0;
//                 ++end;
//             }

//             paddr_t paddr = ht->data + BLK_NUM(ht, idx);
//             ssize_t size  = sizeof(*ht->cache) * (end - idx);
//             off_t offset  = BLK_IDX(ht, idx) * sizeof(*ht->cache);

//             ssize_t ret = CB(ht, cb_write, 
//                              paddr, offset, size, (char*)&(ht->cache[idx]));
//             if(ret != size) {
//                 pthread_rwlock_unlock(ht->cache_lock);
//                 return -EIO;
//             }

//             idx = end;
//         } 
//     }
//     pthread_rwlock_unlock(ht->cache_lock);

//     return 0;
// }

#ifdef __cplusplus
}
#endif

#endif /* __NVM_IDX_G_HASH_MOD_H__ */
