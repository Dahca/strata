#include "global/global.h"

#include "file_indexing.h"
char pwd[MAX_PATH + 1];

indexing_choice_t g_idx_choice;
bool g_idx_cached;

indexing_choice_t get_indexing_choice(void) {
    const char *env = getenv("MLFS_IDX_STRUCT");

    if (env != NULL && !strcmp(env, "EXTENT_TREES")) {
        printf("%s -> using API per-file extent trees!\n", env);
        return EXTENT_TREES;
    } else if (env != NULL && !strcmp(env, "GLOBAL_HASH_TABLE")) {
        printf("%s -> using API global hash table!\n", env);
        return GLOBAL_HASH_TABLE;
    } else if (env != NULL && !strcmp(env, "GLOBAL_CUCKOO_HASH")) {
        printf("%s -> using API global CUCKOO hash table!\n", env);
        return GLOBAL_CUCKOO_HASH;
    } else if (env != NULL && !strcmp(env, "LEVEL_HASH_TABLES")) {
        printf("%s -> using API per-file level hashing!\n", env);
        return LEVEL_HASH_TABLES;
    } else if (env != NULL && !strcmp(env, "RADIX_TREES")) {
        printf("%s -> using API per-file radix trees!\n", env);
        return RADIX_TREES;
    } else if (env == NULL || !strcmp(env, "") || !strcmp(env, "NONE")){
        printf("%s -> using Strata default indexing!\n", env);
    } else {
        panic("Unrecognized indexing structure!");
    }

    return NONE;
}

bool get_indexing_is_cached(void) {
    const char *env = getenv("MLFS_IDX_CACHE");

    if (!env) {
        printf("MLFS_IDX_CACHE not set -> disabling caches by default!\n");
        return false;
    }

    if (!strcmp(env, "1") ||
        !strcmp(env, "TRUE") || !strcmp(env, "true") ||
        !strcmp(env, "YES") || !strcmp(env, "yes")) {
        printf("%s -> using API indexing caching!\n", env);
        return true;
    } 
    
    printf("%s -> disabling caches!\n", env);
    return false;
}

void print_global_idx_stats(bool enable_perf_stats) {
    if (!enable_perf_stats) return;

    idx_fns_t *fns;
    switch(g_idx_choice) {
        case EXTENT_TREES:
            fns = &extent_tree_fns;
            break;
        case RADIX_TREES:
            fns = &radixtree_fns;
            break;
        case LEVEL_HASH_TABLES:
            fns = &levelhash_fns;
            break;
        case GLOBAL_HASH_TABLE:
            fns = &hash_fns;
            break;
        case GLOBAL_CUCKOO_HASH:
            fns = &cuckoohash_fns;
            break;
    }

    if (fns->im_print_global_stats) {
        fns->im_print_global_stats();
    }


    if (fns->im_clean_global_stats) {
        fns->im_clean_global_stats();
    } else {
        fprintf(stderr, "(no clean method for global stats)\n");
    }
}
