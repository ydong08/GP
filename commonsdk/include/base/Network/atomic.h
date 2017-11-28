#ifndef __ATOMIC__T
#define __ATOMIC__T

#include <stdint.h>

#if __GNUC__ < 4
#include "atomic_asm.h"
#else
#include "atomic_gcc.h"
#endif

#if __WORDSIZE==64
#define HAS_ATOMIC8	1
#include "atomic_gcc8.h"
#else
/*
#define HAS_ATOMIC8	1
#include "atomic_asm8.h"
*/
#endif

#define ATOM_CAS(ptr, oval, nval) __sync_bool_compare_and_swap(ptr, oval, nval)
#define ATOM_CAS_POINTER(ptr, oval, nval) __sync_bool_compare_and_swap(ptr, oval, nval)
#define ATOM_INC(ptr) __sync_add_and_fetch(ptr, 1)
#define ATOM_FINC(ptr) __sync_fetch_and_add(ptr, 1)
#define ATOM_DEC(ptr) __sync_sub_and_fetch(ptr, 1)
#define ATOM_FDEC(ptr) __sync_fetch_and_sub(ptr, 1)
#define ATOM_ADD(ptr,n) __sync_add_and_fetch(ptr, n)
#define ATOM_SUB(ptr,n) __sync_sub_and_fetch(ptr, n)
#define ATOM_AND(ptr,n) __sync_and_and_fetch(ptr, n)

#endif
