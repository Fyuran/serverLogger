#ifndef GLIBC_VERSION_H
#define GLIBC_VERSION_H

// Force glibc 2.35 compatibility
#define _GLIBC_MIN_SUPPORTED 0x02230000  // 2.35.0
#define __GLIBC_PREREQ(maj, min) \
    ((__GLIBC__ << 16) + __GLIBC_MINOR__ >= ((maj) << 16) + (min))

#endif // GLIBC_VERSION_H