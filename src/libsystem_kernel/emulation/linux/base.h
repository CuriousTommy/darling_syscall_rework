#ifndef LINUX_BASE_H
#define LINUX_BASE_H

#define VISIBLE __attribute__ ((visibility ("default")))

#define __SYSCALL_NARGS_X(a,b,c,d,e,f,g,h,n,...) n
#define __SYSCALL_NARGS(...) __SYSCALL_NARGS_X(__VA_ARGS__,7,6,5,4,3,2,1,0)
#define __SYSCALL_CONCAT_X(a,b) a##b
#define __SYSCALL_CONCAT(a,b) __SYSCALL_CONCAT_X(a,b)
#define LINUX_SYSCALL(...) __SYSCALL_CONCAT(LINUX_SYSCALL,__SYSCALL_NARGS(__VA_ARGS__))(__VA_ARGS__)

#ifndef BUILDING_BASE_C

#endif /* BUILDING_BASE_C */

#ifdef __x86_64__
#	define LL_ARG(x) (x)
#else
#	define LL_ARG(x) ((union { long long ll; long l[2]; }){ .ll = x }).l[0], \
			((union { long long ll; long l[2]; }){ .ll = x }).l[1]
#endif

#endif

