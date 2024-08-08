#include <stdio.h>
#include <stdint.h>

#ifdef __LP64__
typedef unsigned __int128 cerror_return_t;
#else
typedef uint64_t cerror_return_t;
#endif

//
// HACK: A way for us to test that the error code is being set
//

__thread int darling_errno;

void darling_set_cerror(int err) {
	darling_errno = err;
}

int darling_get_cerror() {
	return darling_errno;
}

__attribute__((noinline))
cerror_return_t
darling_cerror_nocancel(int err)
{
	printf("In cerror_nocancel(%d)\n", err);
	darling_set_cerror(err);
	return -1;
}

__attribute__((noinline))
cerror_return_t
darling_cerror(int err)
{
	printf("In darling_cerror(%d)\n", err);
	return darling_cerror_nocancel(err);
}