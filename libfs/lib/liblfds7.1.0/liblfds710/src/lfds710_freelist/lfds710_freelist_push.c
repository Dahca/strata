/***** includes *****/
#include "lfds710_freelist_internal.h"





/****************************************************************************/
void lfds710_freelist_push( struct lfds710_freelist_state *fs,
                            struct lfds710_freelist_element *fe,
                            struct lfds710_prng_st_state *psts )
{
  char unsigned
    result;

  lfds710_pal_uint_t
    backoff_iteration = LFDS710_BACKOFF_INITIAL_VALUE,
    elimination_array_index,
    loop,
    random_value;

  struct lfds710_freelist_element LFDS710_PAL_ALIGN(LFDS710_PAL_ALIGN_DOUBLE_POINTER)
    *new_top[PAC_SIZE],
    *volatile original_top[PAC_SIZE];

  LFDS710_PAL_ASSERT( fs != NULL );
  LFDS710_PAL_ASSERT( fe != NULL );
  // TRD : psts can be NULL

  LFDS710_MISC_BARRIER_LOAD;

  if( fs->elimination_array_size_in_elements > 0 )
  {
    if( psts != NULL )
    {
      LFDS710_PRNG_ST_GENERATE( *psts, random_value );
      elimination_array_index = ( random_value & (fs->elimination_array_size_in_elements-1) );
    }
    else
    {
      elimination_array_index = (lfds710_pal_uint_t) fe;
      LFDS710_PRNG_ST_MIXING_FUNCTION( elimination_array_index );
      elimination_array_index = ( elimination_array_index & (fs->elimination_array_size_in_elements-1) );
    }

    // TRD : full scan of one cache line, max pointers per cache line

    for( loop = 0 ; loop < LFDS710_FREELIST_ELIMINATION_ARRAY_ELEMENT_SIZE_IN_FREELIST_ELEMENTS ; loop++ )
      if( fs->elimination_array[elimination_array_index][loop] == NULL )
      {
        LFDS710_PAL_ATOMIC_EXCHANGE( &fs->elimination_array[elimination_array_index][loop], fe, struct lfds710_freelist_element * );
        if( fe == NULL )
          return;
      }
  }

  new_top[POINTER] = fe;

  original_top[COUNTER] = fs->top[COUNTER];
  original_top[POINTER] = fs->top[POINTER];

  do
  {
    fe->next = original_top[POINTER];
    LFDS710_MISC_BARRIER_STORE;

    new_top[COUNTER] = original_top[COUNTER] + 1;
    LFDS710_PAL_ATOMIC_DWCAS( fs->top, original_top, new_top, LFDS710_MISC_CAS_STRENGTH_WEAK, result );

    if( result == 0 )
      LFDS710_BACKOFF_EXPONENTIAL_BACKOFF( fs->push_backoff, backoff_iteration );
  }
  while( result == 0 );

  LFDS710_BACKOFF_AUTOTUNE( fs->push_backoff, backoff_iteration );

  return;
}





/****************************************************************************/
void lfds710_freelist_internal_push_without_ea( struct lfds710_freelist_state *fs,
                                                struct lfds710_freelist_element *fe )
{
  char unsigned
    result;

  lfds710_pal_uint_t
    backoff_iteration = LFDS710_BACKOFF_INITIAL_VALUE;

  struct lfds710_freelist_element LFDS710_PAL_ALIGN(LFDS710_PAL_ALIGN_DOUBLE_POINTER)
    *new_top[PAC_SIZE],
    *volatile original_top[PAC_SIZE];

  LFDS710_PAL_ASSERT( fs != NULL );
  LFDS710_PAL_ASSERT( fe != NULL );

  new_top[POINTER] = fe;

  original_top[COUNTER] = fs->top[COUNTER];
  original_top[POINTER] = fs->top[POINTER];

  do
  {
    fe->next = original_top[POINTER];
    LFDS710_MISC_BARRIER_STORE;

    new_top[COUNTER] = original_top[COUNTER] + 1;
    LFDS710_PAL_ATOMIC_DWCAS( fs->top, original_top, new_top, LFDS710_MISC_CAS_STRENGTH_WEAK, result );

    if( result == 0 )
      LFDS710_BACKOFF_EXPONENTIAL_BACKOFF( fs->push_backoff, backoff_iteration );
  }
  while( result == 0 );

  LFDS710_BACKOFF_AUTOTUNE( fs->push_backoff, backoff_iteration );

  return;
}

