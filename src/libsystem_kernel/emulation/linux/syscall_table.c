#include "syscall_table.h"

#include "misc/syscall.h"
#include "unistd/write.h"
#include "unistd/pipe.h"
#include "network/recvfrom.h"

darling_syscall_metadata_t __unix_syscall_table[DARLING_SYSCALLTABLE_UNIX_MAXSIZE] = {
	[0] = { sys_syscall, 0 },
	// [1] = sys_exit,
	// [2] = sys_fork,
	// [3] = sys_read,
	[4] = { sys_write, 3 },
	// [5] = sys_open,
	// [6] = sys_close,
	[29] = { sys_recvfrom, 6 },
	[42] = { sys_pipe, 0 }
};

darling_syscall_metadata_t __machdep_syscall_table[DARLING_SYSCALLTABLE_MACHDEP_MAXSIZE] = {
	// [3] = sys_thread_set_tsd_base,
};

darling_syscall_metadata_t __mach_syscall_table[128] = {
	// [10] = _kernelrpc_mach_vm_allocate_trap_impl,
	// [12] = _kernelrpc_mach_vm_deallocate_trap_impl,
	// [14] = _kernelrpc_mach_vm_protect_trap_impl,
	// [15] = _kernelrpc_mach_vm_map_trap_impl,
	// [16] = _kernelrpc_mach_port_allocate_trap_impl,
	// [17] = _kernelrpc_mach_port_destroy_trap_impl,
	// [18] = _kernelrpc_mach_port_deallocate_trap_impl,
	// [19] = _kernelrpc_mach_port_mod_refs_trap_impl,
	// [20] = _kernelrpc_mach_port_move_member_trap_impl,
};