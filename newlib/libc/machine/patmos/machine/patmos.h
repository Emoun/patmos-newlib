// Copyright 2013 Stefan Hepp
// 
// This file is part of the newlib C library for the Patmos processor.
// 
// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// (COPYING3.LIB). If not, see <http://www.gnu.org/licenses/>.
//
// This file contains various Patmos related functions. 
//

#ifndef _MACHPATMOS_H
#define _MACHPATMOS_H

/**
 * Base address of the CPU info device.
 */
#define __PATMOS_CPUINFO_BASE     0xF0000000
#define __PATMOS_CPUINFO_COREID   (__PATMOS_CPUINFO_BASE + 0x00)
#define __PATMOS_CPUINFO_FREQ     (__PATMOS_CPUINFO_BASE + 0x04)
#define __PATMOS_CPUINFO_CORECNT  (__PATMOS_CPUINFO_BASE + 0x08)

/**
 * Attribute for pointers into the IO-mapped memory. Use as
 *
 * _IODEV int *p = (_IODEV int *) 0x1234;
 *
 * @see _SPM in machine/spm.h
 */
#define _IODEV __attribute__((address_space(1)))

typedef _IODEV unsigned int volatile * const _iodev_ptr_t;

/**
 * Attribute for pointers into main memory using cache bypass. Use as
 *
 * _UNCACHED int *p = (_UNCACHED int *) &mydata;
 */
#define _UNCACHED __attribute__((address_space(3)))


/**
 * Get the CPU ID.
 */
static inline unsigned int get_cpuid()
{
  unsigned int id = *((_iodev_ptr_t)(__PATMOS_CPUINFO_COREID));
  return id;
}

/**
 * Get the CPU frequency.
 */
static inline unsigned int get_cpu_freq()
{
  unsigned int freq = *((_iodev_ptr_t)(__PATMOS_CPUINFO_FREQ));
  return freq;
}

/**
 * Get the number of CPUs.
 */
static inline unsigned int get_cpucnt()
{
  unsigned int cnt = *((_iodev_ptr_t)(__PATMOS_CPUINFO_CORECNT));
  return cnt;
}


/**
 * Base address of the exception unit.
 */
#define __PATMOS_EXCUNIT_BASE       0xF0010000
#define __PATMOS_EXCUNIT_CACHECTRL  (__PATMOS_EXCUNIT_BASE + 0x14)


/**
 * The cache control register (in the exception unit).
 * TODO This should have a PATMOS prefix, as this a header file
 */
#define CACHE_CONTROL (*((_iodev_ptr_t)(__PATMOS_EXCUNIT_CACHECTRL)))

/**
 * Invalidate the data cache.
 */
static inline void inval_dcache() __attribute__((section(".text.spm")));
static inline void inval_dcache()
{
  CACHE_CONTROL = 0x01;
}

/**
 * Invalidate the method cache.
 *
 * Requires that the .text.spm section is put into the I-SPM address range
 * by the linker.
 */
static inline void inval_mcache() __attribute__((section(".text.spm")));
static inline void inval_mcache()
{
  CACHE_CONTROL = 0x02;
}

/**
 * Invalidate the stack cache by resetting the spill pointer to the top-of-stack.
 *
 * TODO Test if this works in all cases and stack implementations.
 *
 * This relies on the fact that the call site will ensure (and thus fill back) 
 * its required stack size from memory after this function returns.
 */
static void inval_scache() __attribute__((naked, section(".text.spm")));
static void inval_scache()
{
  asm volatile (
       "mfs $r1 = $st;\n\t"
       "mts $ss = $r1;\n\t"
       : : : "$r1");
}


/**
 * Spill all data in the stack cache to memory, emptying the stack cache.
 *
 * This relies on the fact that the call site will ensure (and thus fill back) 
 * its required stack size after this function returns.
 */
static void flush_scache() __attribute__((naked, section(".text.spm")));
static void flush_scache()
{
  asm volatile (
       "mfs $r1 = $ss;\n\t"
       "mfs $r2 = $st;\n\t"
       "sub $r1 = $r1, $r2\n\t"
       "sspill $r1\n\t"
       : : : "$r1", "$r2");
}

/**
 * Invalidate all caches.
 *
 * Requires that the .text.spm section is put into the I-SPM address range
 * by the linker.
 */
static inline void inval_caches() __attribute__((section(".text.spm")));
static inline void inval_caches()
{
  inval_scache();
  // Clear the method cache (and data cache) last, in case flush_scache is not
  // in the SPM.
  CACHE_CONTROL = 0x01 | 0x02;
}


/**
 * Clear all caches, by flushing the S$ and invalidating the D$ and M$.
 *
 * Requires that the .text.spm section is put into the I-SPM address range
 * by the linker.
 */
static inline void clear_caches() __attribute__((section(".text.spm")));
static inline void clear_caches()
{
  flush_scache();
  // Clear the method cache (and data cache) last, in case flush_scache is not
  // in the SPM.
  CACHE_CONTROL = 0x01 | 0x02;
}

#endif /* _MACHPATMOS_H */
