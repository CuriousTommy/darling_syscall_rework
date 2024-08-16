
#include "vchroot_expand.h"

#include <stdbool.h>

typedef struct linux_stat stat_t;

struct _xlocale;
typedef struct _xlocale* locale_t;

#define LC_C_LOCALE ((locale_t)NULL)

#define LINUX_S_IFMT 00170000
#define LINUX_S_IFLNK 0120000

static bool next_component(const char* from, const char** end);

static char prefix_path[4096] = {0}; // MUST NOT end with '/'
static int prefix_path_len = -1;

static const char EXIT_PATH[] = "/Volumes/SystemRoot";

static const int MAX_SYMLINK_DEPTH = 10;

struct context
{
	const char* current_root;
	int current_root_len;

	char current_path[4096];
	int current_path_len;

	int symlink_depth;
	bool unknown_component;
	bool follow;
};

static void init_vchroot_path(void)
{

}

int vchroot_expand(struct vchroot_expand_args* args)
{
	return 0;
}

static int vchroot_run(const char* input_path, struct context* ctxt)
{
	return 0;
}

static bool next_component(const char* from, const char** end)
{
	return true;
}

int vchroot_fdpath(struct vchroot_fdpath_args* args)
{
	return 0;
}

int vchroot_unexpand(struct vchroot_unexpand_args* args)
{
	return 0;
}
