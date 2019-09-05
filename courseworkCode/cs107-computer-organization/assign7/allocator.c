/*
 * File: allocator.c
 * Author: YOUR NAME HERE
 * ----------------------
 * A trivial allocator. Very simple code, heinously inefficient.
 * An allocation request is serviced by incrementing the heap segment
 * to place new block on its own page(s). The block has a pre-node header
 * containing size information. Free is a no-op: blocks are never coalesced
 * or reused. Realloc is implemented using malloc/memcpy/free.
 * Using page-per-node means low utilization. Because it grows the heap segment
 * (using expensive OS call) for each node, it also has low throughput
 * and terrible cache performance.  The code is not robust in terms of
 * handling unusual cases either.
 *
 * In short, this implementation has not much going for it other than being
 * the smallest amount of code that "works" for ordinary cases.  But
 * even that's still a good place to start from.
 */

#include <stdlib.h>
#include <string.h>
#include "allocator.h"
#include "segment.h"

// Heap blocks are required to be aligned to 8-byte boundary
#define ALIGNMENT 8

typedef struct {
    size_t payloadsz;   // header contains just one 8-byte field
} headerT;

// Very efficient bitwise round of sz up to nearest multiple of mult
// does this by adding mult-1 to sz, then masking off the
// the bottom bits to compute least multiple of mult that is
// greater/equal than sz, this value is returned
// NOTE: mult has to be power of 2 for the bitwise trick to work!
static inline size_t roundup(size_t sz, size_t mult)
{
    return (sz + mult-1) & ~(mult-1);
}


// Given a pointer to start of payload, simply back up
// to access its block header
static inline headerT *hdr_for_payload(void *payload)
{
    return (headerT *)((char *)payload - sizeof(headerT));
}


// Given a pointer to block header, advance past
// header to access start of payload
static inline void *payload_for_hdr(headerT *header)
{
    return (char *)header + sizeof(headerT);
}


/* The responsibility of the myinit function is to configure a new
 * empty heap. Typically this function will initialize the
 * segment (you decide the initial number pages to set aside, can be
 * zero if you intend to defer until first request) and set up the
 * global variables for the empty, ready-to-go state. The myinit
 * function is called once at program start, before any allocation 
 * requests are made. It may also be called later to wipe out the current
 * heap contents and start over fresh. This "reset" option is specifically
 * needed by the test harness to run a sequence of scripts, one after another,
 * without restarting program from scratch.
 */
bool myinit()
{
    init_heap_segment(0); // reset heap segment to empty, no pages allocated
    return true;
}


// malloc a block by rounding up size to number of pages, extending heap
// segment and using most recently added page(s) for this block. This
// means each block gets its own page -- how generous! :-)
void *mymalloc(size_t requestedsz)
{
    size_t npages = roundup(requestedsz + sizeof(headerT), PAGE_SIZE)/PAGE_SIZE;
    headerT *header = extend_heap_segment(npages);
    header->payloadsz = requestedsz;
    return payload_for_hdr(header);
}


// free does nothing.  fast!... but lame :(
void myfree(void *ptr)
{
}


// realloc built on malloc/memcpy/free is easy to write.
// This code will work ok on ordinary cases, but needs attention
// to robustness. Realloc efficiency can be improved by
// implementing a standalone realloc as opposed to
// delegating to malloc/free.
void *myrealloc(void *oldptr, size_t newsz)
{
    void *newptr = mymalloc(newsz);
    size_t oldsz = hdr_for_payload(oldptr)->payloadsz;
    memcpy(newptr, oldptr, oldsz < newsz ? oldsz: newsz);
    myfree(oldptr);
    return newptr;
}


// validate_heap is your debugging routine to detect/report
// on problems/inconsistency within your heap data structures
bool validate_heap()
{
    return true;
}

