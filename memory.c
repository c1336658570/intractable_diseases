# if 0
struct malloc_state
{
  /* Serialize access.  */
  __libc_lock_define (, mutex);

  /* Flags (formerly in max_fast).  */
  int flags;

  /* Set if the fastbin chunks contain recently inserted free blocks.  */
  /* Note this is a bool but not all targets support atomics on booleans.  */
  int have_fastchunks;

  /* Fastbins */
  mfastbinptr fastbinsY[NFASTBINS];

  /* Base of the topmost chunk -- not otherwise kept in a bin */
  mchunkptr top;

  /* The remainder from the most recent split of a small request */
  mchunkptr last_remainder;

  /* Normal bins packed as described above */
  mchunkptr bins[NBINS * 2 - 2];

  /* Bitmap of bins */
  unsigned int binmap[BINMAPSIZE];

  /* Linked list */
  struct malloc_state *next;

  /* Linked list for free arenas.  Access to this field is serialized
     by free_list_lock in arena.c.  */
  struct malloc_state *next_free;

  /* Number of threads attached to this arena.  0 if the arena is on
     the free list.  Access to this field is serialized by
     free_list_lock in arena.c.  */
  INTERNAL_SIZE_T attached_threads;

  /* Memory allocated from the system in this arena.  */
  INTERNAL_SIZE_T system_mem;
  INTERNAL_SIZE_T max_system_mem;
};

struct malloc_chunk {

  INTERNAL_SIZE_T      mchunk_prev_size;  /* Size of previous chunk (if free).  */
  INTERNAL_SIZE_T      mchunk_size;       /* Size in bytes, including overhead. */

  struct malloc_chunk* fd;         /* double links -- used only if free. */
  struct malloc_chunk* bk;

  /* Only used for large blocks: pointer to next larger size.  */
  struct malloc_chunk* fd_nextsize; /* double links -- used only if free. */
  struct malloc_chunk* bk_nextsize;
};

typedef struct _heap_info
{
  mstate ar_ptr; /* Arena for this heap. */
  struct _heap_info *prev; /* Previous heap. */
  size_t size;   /* Current size in bytes. */
  size_t mprotect_size; /* Size in bytes that has been mprotected
                           PROT_READ|PROT_WRITE.  */
  size_t pagesize; /* Page size used when allocating the arena.  */
  /* Make sure the following data is properly aligned, particularly
     that sizeof (heap_info) + 2 * SIZE_SZ is a multiple of
     MALLOC_ALIGNMENT. */
  char pad[-3 * SIZE_SZ & MALLOC_ALIGN_MASK];
} heap_info;
#endif
/**
 * 一个线程包含一个arena(struct malloc_state)，arena有数量限制，arena下有多个堆(struct _heap_info)
 * 每个arena下有多个struct malloc_state
*/


#include <stdio.h>
#include <stdlib.h>

struct malloc_chunk {

  size_t      mchunk_prev_size;  /* Size of previous chunk (if free).  */
  size_t      mchunk_size;       /* Size in bytes, including overhead. */

  struct malloc_chunk* fd;         /* double links -- used only if free. */
  struct malloc_chunk* bk;

  /* Only used for large blocks: pointer to next larger size.  */
  struct malloc_chunk* fd_nextsize; /* double links -- used only if free. */
  struct malloc_chunk* bk_nextsize;
};

int main(void) {
  while (1);
  char *a = (char *)malloc(30);
  char *b = (char *)malloc(31);
  char *c = (char *)malloc(32);
  char *d = (char *)(malloc(56));

  // char *d = (char *)(malloc(63));
  char *e = (char *)(malloc(64));
  char *k = (char *)(malloc(64));
  char *q = (char *)(malloc(64));
  char *m = (char *)(malloc(64));

  // free(a+1);

  char *f = (char *)malloc(513);
  char *g = (char *)malloc(514);

  printf("%p\n%p\n%p\n%p\n%p\n%p\n%p\n", a, b, c, d, e, f, g);

  printf("a = %lu ", (*(size_t *)(a - 16)) &  ~(sizeof(size_t) - 1));
  printf("%lu\n", *(size_t *)(a - 8) &  ~(sizeof(size_t) - 1));
  printf("b = %lu ", *(size_t *)(b - 16) &  ~(sizeof(size_t) - 1));
  printf("%lu\n", *(size_t *)(b - 8) &  ~(sizeof(size_t) - 1));
  printf("c = %lu ", *(size_t *)(c - 16) &  ~(sizeof(size_t) - 1));
  printf("%lu\n", *(size_t *)(c - 8)&  ~(sizeof(size_t) - 1));
  printf("d = %lu ", *(size_t *)(d - 16) &  ~(sizeof(size_t) - 1));
  printf("%lu\n", *(size_t *)(d - 8) &  ~(sizeof(size_t) - 1)); // 为什么是64？？？，64-56 == 8啊
  printf("e = %lu ", *(size_t *)(e - 16) &  ~(sizeof(size_t) - 1));
  printf("%lu\n", *(size_t *)(e - 8) &  ~(sizeof(size_t) - 1));
  printf("f = %lu ", *(size_t *)(f - 16) &  ~(sizeof(size_t) - 1));
  printf("%lu\n", *(size_t *)(f - 8) &  ~(sizeof(size_t) - 1));
  printf("g = %lu ", *(size_t *)(g - 16) &  ~(sizeof(size_t) - 1));
  printf("%lu\n", *(size_t *)(g - 8) &  ~(sizeof(size_t) - 1));

  free(e);
  free(q);
  free(k);
  free(m);
  free(f);
  printf("e = %lu ", *(size_t *)(e - 16) &  ~(sizeof(size_t) - 1));
  printf("%lu\n", *(size_t *)(e - 8) &  ~(sizeof(size_t) - 1));
  printf("e = %lu ", *(size_t *)(e - 16) &  ~(sizeof(size_t) - 1));
  printf("%lu\n", *(size_t *)(e - 8) &  ~(sizeof(size_t) - 1));
  for (int i = 0; i < 64; ++i) {
    e[i] = 0;
  }
  printf("%lu %lu\n", ((struct malloc_chunk *)(e - 16))->mchunk_prev_size,((struct malloc_chunk *)(e - 16))->mchunk_size);

  return 0;
}