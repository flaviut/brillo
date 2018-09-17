#include "common.h"
#include "helpers.h"
#include "ctrl.h"
#include "light.h"
#include "info.h"

static bool info_list(void);

/**
 * info_print:
 * @exec:	whether or not to take action
 *
 * If exec is true, prints information
 * according to the operation mode.
 *
 * Returns: true if op_mode is an info mode, otherwise false
 **/
bool info_print(bool exec)
{
	switch (light_conf.op_mode) {
		case LIGHT_PRINT_HELP:
			if (exec)
				info_print_help();
			break;
		case LIGHT_PRINT_VERSION:
			if (exec)
				info_print_version();
			break;
		case LIGHT_LIST_CTRL:
			if (exec)
				info_list();
			break;
		default:
			return false;
	}

	return true;
}

/**
 * info_list:
 *
 * Prints controller names for the appropriate target.
 *
 * Returns: false if could not list controllers or no
 * 		controllers found, otherwise true
 **/
bool info_list()
{
	DIR *dir;

	dir = opendir(light_conf.sys_prefix);

	if (!dir) {
		LIGHT_ERR("opendir: %s", strerror(errno));
		return false;
	}

	for (char *c; (c = light_ctrl_iter_next(dir)); free(c))
		printf("%s\n", c);

	closedir(dir);
	return true;
}

/**
 * info_print_version:
 *
 * Prints version and copyright information to standard output.
 **/
void info_print_version()
{
	printf("%s %u.%u (%s)\n", LIGHT_PROG, LIGHT_VER_MAJOR, LIGHT_VER_MINOR,
	       LIGHT_VER_TYPE);
	printf("Copyright (C) %u %s, ", LIGHT_VENDOR_YEAR, LIGHT_VENDOR);
	printf("%u %s\n", LIGHT_YEAR, LIGHT_AUTHOR);
	printf
	    ("This is free software, see the source for copying conditions.  There is NO\n");
	printf
	    ("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE\n");
}

/**
 * info_print_help:
 *
 * Prints help dialog to standard output.
 **/
void info_print_help()
{
	printf("Usage: %s [operation] [value] [-k] [-r] [-m|-c] [-s controller] [-v loglevel]\n", LIGHT_PROG);
	printf("Operations (can not be used in conjunction):\n");
	printf("  -H -h:\tPrints this help and exits\n");
	printf("  -V:\t\tPrints version info and exits\n");
	printf("  -G:\t\tGet value (default)\n");
	printf("  -S:\t\tSet value\n");
	printf("  -A:\t\tAdd value\n");
	printf("  -U:\t\tSubtract value\n");
	printf("  -L:\t\tList controllers\n");
	printf("  -I:\t\tRestore brightness\n");
	printf("  -O:\t\tSave brightness\n\n");

	printf("Targets (can not be used in conjunction):\n");
	printf("  -l:\t\tAct on screen backlight (default)\n");
	printf("  -k:\t\tAct on keyboard backlight\n\n");

	printf("Fields (can not be used in conjunction):\n");
	printf("  -b:\t\tBrightness (default) (used with [GSAU])\n");
	printf("  -m:\t\tMaximum brightness (used with [G])\n");
	printf("  -c:\t\tMinimum cap (used with [GS])\n\n");

	printf("Controller selection (can not be used in conjunction):\n");
	printf("  -a:\t\tSelects controller automatically (default).\n");
	printf("  -s:\t\tSpecify controller to use. (needs argument)\n\n");

	printf("Value modes (can not be used in conjunction):\n");
	printf
	    ("  -p:\t\tInterpret <value> as, and output values in, percent. (default)\n");
	printf
	    ("  -r:\t\tInterpret <value> as, and output values in, raw mode.\n\n");

	printf("Other:\n");
	printf
	    ("  -v:\t\tSets the verbosity level, (needs argument).\n  \t\t0: Only outputs read values.\n  \t\t1: Read values, Errors.\n  \t\t2: Read values, Errors, Warnings.\n  \t\t3: Read values, Errors, Warnings, Notices.\n\n");
}