#ifndef _BSDTHREAD_CANCELLABLE_H
#define _BSDTHREAD_CANCELLABLE_H
#include <sys/errno.h>
// #include "pthread_canceled.h"

// HACK: Let's not worry about the CANCELATION_POINT stuff for our POC

// #define CANCELATION_POINT() \
// 	if (sys_pthread_canceled(0) == 0) \
// 		return -EINTR;

#define CANCELATION_POINT()

#endif

