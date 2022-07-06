//  (C) Copyright John Maddock 2001 - 2003.
//  (C) Copyright Darin Adler 2001.
//  (C) Copyright Douglas Gregor 2002.
//  (C) Copyright Chris Johns 2022.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  RTEMS config options:

#if !defined(__rtems__)
#error "This platform is not RTEMS"
#endif

#define BOOST_PLATFORM "RTEMS " BOOST_STRINGIZE(__rtems__)

#define BOOST_NO_INTRINSIC_WCHAR_T

#define BOOST_FILESYSTEM_DISABLE_SENDFILE
#define BOOST_FILESYSTEM_DISABLE_COPY_FILE_RANGE
#define BOOST_FILESYSTEM_DISABLE_STATX

#define BOOST_HAS_PTHREADS
#define BOOST_NO_CWCHAR
#define BOOST_HAS_SCHED_YIELD
#define BOOST_HAS_NANOSLEEP
#define BOOST_HAS_GETTIMEOFDAY
#define BOOST_HAS_PTHREAD_MUTEXATTR_SETTYPE
#define BOOST_HAS_CLOCK_GETTIME
#define BOOST_HAS_UNISTD_H
#include <boost/config/detail/posix_features.hpp>
