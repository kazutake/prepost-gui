/* ------------------------------------------------------------------------- *
 * CGNS - CFD General Notation System (http://www.cgns.org)                  *
 * CGNS/MLL - Mid-Level Library header file                                  *
 * Please see cgnsconfig.h file for this local installation configuration    *
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *

  This software is provided 'as-is', without any express or implied warranty.
  In no event will the authors be held liable for any damages arising from
  the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.

  2. Altered source versions must be plainly marked as such, and must not
     be misrepresented as being the original software.

  3. This notice may not be removed or altered from any source distribution.

 * ------------------------------------------------------------------------- */

#ifndef CGNSTYPES_H
#define CGNSTYPES_H

#define CG_BUILD_LEGACY 0
#define CG_BUILD_64BIT  0
#define CG_BUILD_SCOPE  0

#define CG_MAX_INT32 0x7FFFFFFF
#ifdef _WIN32
# define CG_LONG_T __int64
#else
# define CG_LONG_T long long
#endif

#if CG_BUILD_LEGACY
# define CG_SIZEOF_SIZE    32
# define CG_SIZE_DATATYPE "I4"
# define cgerr_t  int
# define cgint_t  int
# define cgsize_t int
# define cgid_t   double
#else
# if CG_BUILD_64BIT
#  define CG_SIZEOF_SIZE    64
#  define CG_SIZE_DATATYPE "I8"
   typedef CG_LONG_T cgsize_t;
# else
#  define CG_SIZEOF_SIZE    32
#  define CG_SIZE_DATATYPE "I4"
   typedef int cgsize_t;
# endif
  typedef int cgerr_t;
  typedef int cgint_t;
  typedef double cgid_t;
#endif

typedef CG_LONG_T cglong_t;
typedef unsigned CG_LONG_T cgulong_t;

#endif
