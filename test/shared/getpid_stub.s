#if defined(__x86_64__)
.data
.private_extern __darling_current_pid
__darling_current_pid:
	.long 0

#else
#error Unsupported architecture
#endif