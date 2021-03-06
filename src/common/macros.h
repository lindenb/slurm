/*****************************************************************************\
 * src/common/macros.h - some standard macros for slurm
 * $Id$
 *****************************************************************************
 *  Copyright (C) 2002 The Regents of the University of California.
 *  Produced at Lawrence Livermore National Laboratory (cf, DISCLAIMER).
 *  Written by Mark Grondona <mgrondona@llnl.gov>.
 *  CODE-OCEC-09-009. All rights reserved.
 *
 *  This file is part of SLURM, a resource management program.
 *  For details, see <http://slurm.schedmd.com/>.
 *  Please also read the included file: DISCLAIMER.
 *
 *  SLURM is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your option)
 *  any later version.
 *
 *  In addition, as a special exception, the copyright holders give permission
 *  to link the code of portions of this program with the OpenSSL library under
 *  certain conditions as described in each individual source file, and
 *  distribute linked combinations including the two. You must obey the GNU
 *  General Public License in all respects for all of the code used other than
 *  OpenSSL. If you modify file(s) with this exception, you may extend this
 *  exception to your version of the file(s), but you are not obligated to do
 *  so. If you do not wish to do so, delete this exception statement from your
 *  version.  If you delete this exception statement from all source files in
 *  the program, then also delete it here.
 *
 *  SLURM is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with SLURM; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA.
\*****************************************************************************/

#ifndef _MACROS_H
#define _MACROS_H 	1

#if HAVE_CONFIG_H
#  include "config.h"
#endif

#ifndef NULL
#  include <stddef.h>	/* for NULL */
#endif

#if HAVE_STDBOOL_H
#  include <stdbool.h>
#else
#  ifndef bool
     typedef enum {false, true} bool;
#  endif
#endif /* !HAVE_STDBOOL_H */

#if HAVE_PTHREAD_H
#  include <pthread.h>
#endif

#include <errno.h>              /* for errno   */
#include "src/common/log.h"	/* for error() */

#ifndef FALSE
#  define FALSE	false
#endif

#ifndef TRUE
#  define TRUE	true
#endif

#ifndef MAX
#  define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#  define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

/*
 * NOTE: ISO C doesn't guarantee that the following works, but POSIX does,
 * as well as Windows and all reasonable systems. For maximum portability,
 * one should do:
 * SLURM_DIFFTIME(a,b) difftime((a), (b))
 * but this code can show up high in the profile, so use the faster
 * (in principle unportable but in practice fine) code below.
 */
#define SLURM_DIFFTIME(a,b) ((a) - (b))

/* Avoid going over 32 bits for a constant to avoid warnings on some systems */
#  define UINT64_SWAP_LE_BE(val)      ((uint64_t) (                           \
        (((uint64_t) (val) &                                                  \
          (uint64_t) (0x00000000000000ffU)) << 56) |                          \
        (((uint64_t) (val) &                                                  \
          (uint64_t) (0x000000000000ff00U)) << 40) |                          \
        (((uint64_t) (val) &                                                  \
          (uint64_t) (0x0000000000ff0000U)) << 24) |                          \
        (((uint64_t) (val) &                                                  \
          (uint64_t) (0x00000000ff000000U)) <<  8) |                          \
	(((uint64_t) (val)                  >>  8) &                          \
	  (uint64_t) (0x00000000ff000000U))        |                          \
	(((uint64_t) (val)                  >> 24) &                          \
	  (uint64_t) (0x0000000000ff0000U))        |                          \
	(((uint64_t) (val)                  >> 40) &                          \
	  (uint64_t) (0x000000000000ff00U))        |                          \
	(((uint64_t) (val)                  >> 56) &                          \
	  (uint64_t) (0x00000000000000ffU)) ))

#if SLURM_BIGENDIAN
# define HTON_int64(x)	  ((int64_t)  (x))
# define NTOH_int64(x)	  ((int64_t)  (x))
# define HTON_uint64(x)	  ((uint64_t) (x))
# define NTOH_uint64(x)	  ((uint64_t) (x))
#else
# define HTON_int64(x)    ((int64_t) UINT64_SWAP_LE_BE (x))
# define NTOH_int64(x)	  ((int64_t) UINT64_SWAP_LE_BE (x))
# define HTON_uint64(x)   UINT64_SWAP_LE_BE (x)
# define NTOH_uint64(x)   UINT64_SWAP_LE_BE (x)
#endif	/* SLURM_BIGENDIAN */



/*
** define __CURRENT_FUNC__ macro for returning current function
*/
#if defined (__GNUC__) && (__GNUC__ < 3)
#  define __CURRENT_FUNC__	__PRETTY_FUNCTION__
#else  /* !__GNUC__ */
#  ifdef _AIX
#    define __CURRENT_FUNC__	__func__
#  else
#    define __CURRENT_FUNC__    ""
#  endif /* _AIX */
#endif /* __GNUC__ */

#ifndef __STRING
#  define __STRING(arg)		#arg
#endif

/* define macros for GCC function attributes if we're using gcc */

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 4)
#  define __PRINTF_ATTR( form_idx, arg_idx ) 	\
          __attribute__((__format__ (__printf__, form_idx, arg_idx)))
#  define __NORETURN_ATTR				\
          __attribute__((__noreturn__))
#else  /* !__GNUC__ */
#  define __PRINTF_ATTR( format_idx, arg_idx )	((void)0)
#  define __NORETURN_ATTR			((void)0)
#endif /* __GNUC__ */

/* the following is taken directly from glib 2.0, with minor changes */

/* Provide simple macro statement wrappers (adapted from Perl):
 *  _STMT_START { statements; } _STMT_END;
 *  can be used as a single statement, as in
 *  if (x) _STMT_START { ... } _STMT_END; else ...
 *
 *  For gcc we will wrap the statements within `({' and `})' braces.
 *  For SunOS they will be wrapped within `if (1)' and `else (void) 0',
 *  and otherwise within `do' and `while (0)'.
 */
#if !(defined (_STMT_START) && defined (_STMT_END))
#  if defined (__GNUC__) && !defined (__STRICT_ANSI__) && !defined (__cplusplus)
#    define _STMT_START        (void)(
#    define _STMT_END          )
#  else
#    if (defined (sun) || defined (__sun__))
#      define _STMT_START      if (1)
#      define _STMT_END        else (void)0
#    else
#      define _STMT_START      do
#      define _STMT_END        while (0)
#    endif
#  endif
#endif

#ifdef WITH_PTHREADS

#  define slurm_mutex_init(mutex)                                             \
     _STMT_START {                                                            \
         int err = pthread_mutex_init(mutex, NULL);                           \
         if (err) {                                                           \
             errno = err;                                                     \
             error("%s:%d %s: pthread_mutex_init(): %m",                      \
                   __FILE__, __LINE__, __CURRENT_FUNC__);                     \
         }                                                                    \
     } _STMT_END

#  define slurm_mutex_destroy(mutex)                                          \
     _STMT_START {                                                            \
         int err = pthread_mutex_destroy(mutex);                              \
         if (err) {                                                           \
             errno = err;                                                     \
             error("%s:%d %s: pthread_mutex_destroy(): %m",                   \
                   __FILE__, __LINE__, __CURRENT_FUNC__);                     \
         }                                                                    \
     } _STMT_END

#  define slurm_mutex_lock(mutex)                                             \
     _STMT_START {                                                            \
         int err = pthread_mutex_lock(mutex);                                 \
         if (err) {                                                           \
             errno = err;                                                     \
             error("%s:%d %s: pthread_mutex_lock(): %m",                      \
                   __FILE__, __LINE__, __CURRENT_FUNC__);                     \
         }                                                                    \
     } _STMT_END

#  define slurm_mutex_unlock(mutex)                                           \
     _STMT_START {                                                            \
         int err = pthread_mutex_unlock(mutex);                               \
         if (err) {                                                           \
             errno = err;                                                     \
             error("%s:%d %s: pthread_mutex_unlock(): %m",                    \
                   __FILE__, __LINE__, __CURRENT_FUNC__);                     \
         }                                                                    \
     } _STMT_END

#  ifdef PTHREAD_SCOPE_SYSTEM
#  define slurm_attr_init(attr)                                               \
     _STMT_START {                                                            \
	if (pthread_attr_init(attr))                                          \
		fatal("pthread_attr_init: %m");                               \
	/* we want 1:1 threads if there is a choice */                        \
	if (pthread_attr_setscope(attr, PTHREAD_SCOPE_SYSTEM))                \
		error("pthread_attr_setscope: %m");                           \
	if (pthread_attr_setstacksize(attr, 1024*1024))                       \
		error("pthread_attr_setstacksize: %m");                       \
     } _STMT_END
#  else
#  define slurm_attr_init(attr)                                               \
     _STMT_START {                                                            \
        if (pthread_attr_init(attr))                                          \
                fatal("pthread_attr_init: %m");                               \
        if (pthread_attr_setstacksize(attr, 1024*1024))                       \
                error("pthread_attr_setstacksize: %m");                       \
     } _STMT_END
#  endif

#  define slurm_attr_destroy(attr)					      \
     _STMT_START {                                                            \
        if (pthread_attr_destroy(attr))                                       \
             error("pthread_attr_destroy failed, possible memory leak!: %m"); \
     } _STMT_END

#else /* !WITH_PTHREADS */

#  define slurm_mutex_init(mutex)
#  define slurm_mutex_destroy(mutex)
#  define slurm_mutex_lock(mutex)
#  define slurm_mutex_unlock(mutex)
#  define slurm_attr_init(attr)
#  define slurm_attr_destroy(attr)

#endif /* WITH_PTHREADS */

#define slurm_atoul(str) strtoul(str, NULL, 10)
#define slurm_atoull(str) strtoull(str, NULL, 10)

#ifndef strong_alias
#  if USE_ALIAS
#    define strong_alias(name, aliasname) \
     extern __typeof (name) aliasname __attribute ((alias (#name)))
#  else
     /* dummy function definition,
      * confirm "aliasname" is free and waste "name" */
#    define strong_alias(name, aliasname) \
     extern void aliasname(int name)
#  endif
#endif

#ifndef HAVE_STRNDUP
#  undef  strndup
#  define strndup(src,size) strdup(src)
#endif

/* Results strftime() are undefined if buffer too small
 * This variant returns a string of "####"... instead */
#define slurm_strftime(s, max, format, tm)				\
_STMT_START {								\
	if (max > 0) {							\
		char tmp_string[(max<256?256:max+1)];			\
		if (strftime(tmp_string, sizeof(tmp_string), format, tm) == 0) \
			memset(tmp_string, '#', max);			\
		tmp_string[max-1] = 0;					\
		strncpy(s, tmp_string, max);				\
	}								\
} _STMT_END

/* There are places where we put NO_VAL or INFINITE into a float or double
 * Use fuzzy_equal below to test for those values rather than an comparision
 * which could fail due to rounding errors. */
#define FUZZY_EPSILON 0.00001
#define fuzzy_equal(v1, v2) ((((v1)-(v2)) > -FUZZY_EPSILON) && (((v1)-(v2)) < FUZZY_EPSILON))

#endif /* !_MACROS_H */
