#include <getopt.h>

#include "common.h"
#include "helpers.h"
#include "path.h"
#include "info.h"
#include "ctrl.h"
#include "light.h"

#define ASSERT_SET(t,v) \
  if(v)\
  {\
    fprintf(stderr, t" arguments can not be used in conjunction.\n");\
    return false;\
  }\
  v = true;

#define ASSERT_OPSET() ASSERT_SET("Operation", opSet)
#define ASSERT_TARGETSET() ASSERT_SET("Target", targetSet)
#define ASSERT_FIELDSET() ASSERT_SET("Field", fieldSet)
#define ASSERT_CTRLSET() ASSERT_SET("Controller", ctrlSet)
#define ASSERT_VALSET() ASSERT_SET("Value", valSet)

/**
 * light_defaults:
 *
 * Initialize the default configuration values.
 **/
void light_defaults()
{
	light_conf.ctrl_mode = LIGHT_AUTO;
	light_conf.ctrl = NULL;
	light_conf.op_mode = LIGHT_GET;
	light_conf.val_mode = LIGHT_PERCENT;
	light_conf.val_raw = 0;
	light_conf.val_pct = 0.0;
	light_conf.target = LIGHT_BACKLIGHT;
	light_conf.field = LIGHT_BRIGHTNESS;
	light_conf.cached_max = 0;
	light_loglevel = 0;
}

/**
 * light_check_ops:
 *
 * Ensure that the op_mode is valid for the configuration's field.
 *
 * Returns: false if an invalid operation mode is used, otherwise true.
 **/
bool light_check_ops()
{
	bool valid = true;
	LIGHT_OP_MODE op = light_conf.op_mode;

	/* Nothing to check if we just print info */
	if (op == LIGHT_PRINT_HELP || op == LIGHT_PRINT_VERSION
	    || op == LIGHT_LIST_CTRL) {
		return true;
	}

	switch (light_conf.field) {
	case LIGHT_BRIGHTNESS:
		if (op != LIGHT_GET && op != LIGHT_SET &&
		    op != LIGHT_ADD && op != LIGHT_SUB &&
		    op != LIGHT_SAVE && op != LIGHT_RESTORE) {
			valid = false;
			fprintf(stderr,
				"Wrong operation specified for brightness. You can use only -G -S -A or -U\n\n");
		}
		break;
	case LIGHT_MAX_BRIGHTNESS:
		if (op != LIGHT_GET) {
			valid = false;
			fprintf(stderr,
				"Wrong operation specified for max brightness. You can only use -G\n\n");
		}
		break;
	case LIGHT_MIN_CAP:
		if (op != LIGHT_GET && op != LIGHT_SET) {
			valid = false;
			fprintf(stderr,
				"Wrong operation specified for min cap. You can only use -G or -S\n\n");
		}
	default:
		break;
	}
	return valid;
}

/**
 * light_parse_args:
 * @argc	argument count
 * @argv	argument array
 *
 * WARNING: may allocate a string in light_conf.ctrl,
 *          but will not free it
 *
 * Returns: true on success, false on failure
 **/
bool light_parse_args(int argc, char **argv)
{
	int opt;
	int verbosity;

	bool opSet = false;
	bool targetSet = false;
	bool fieldSet = false;
	bool ctrlSet = false;
	bool valSet = false;

	light_defaults();

	while ((opt = getopt(argc, argv, "HhVGSAULIObmclkas:prv:")) != -1) {
		switch (opt) {
			/* -- Operations -- */
		case 'H':
		case 'h':
			ASSERT_OPSET();
			light_conf.op_mode = LIGHT_PRINT_HELP;
			break;
		case 'V':
			ASSERT_OPSET();
			light_conf.op_mode = LIGHT_PRINT_VERSION;
			break;
		case 'G':
			ASSERT_OPSET();
			light_conf.op_mode = LIGHT_GET;
			break;
		case 'S':
			ASSERT_OPSET();
			light_conf.op_mode = LIGHT_SET;
			break;
		case 'A':
			ASSERT_OPSET();
			light_conf.op_mode = LIGHT_ADD;
			break;
		case 'U':
			ASSERT_OPSET();
			light_conf.op_mode = LIGHT_SUB;
			break;
		case 'L':
			ASSERT_OPSET();
			light_conf.op_mode = LIGHT_LIST_CTRL;
			break;
		case 'I':
			ASSERT_OPSET();
			light_conf.op_mode = LIGHT_RESTORE;
			break;
		case 'O':
			ASSERT_OPSET();
			light_conf.op_mode = LIGHT_SAVE;
			break;

			/* -- Targets -- */
		case 'l':
			ASSERT_TARGETSET();
			light_conf.target = LIGHT_BACKLIGHT;
			break;
		case 'k':
			ASSERT_TARGETSET();
			light_conf.target = LIGHT_KEYBOARD;
			break;

			/* -- Fields -- */
		case 'b':
			ASSERT_FIELDSET();
			light_conf.field = LIGHT_BRIGHTNESS;
			break;
		case 'm':
			ASSERT_FIELDSET();
			light_conf.field = LIGHT_MAX_BRIGHTNESS;
			break;
		case 'c':
			ASSERT_FIELDSET();
			light_conf.field = LIGHT_MIN_CAP;
			break;

			/* -- Controller selection -- */
		case 'a':
			ASSERT_CTRLSET();
			light_conf.ctrl_mode = LIGHT_AUTO;
			break;;
		case 's':
			ASSERT_CTRLSET();
			light_conf.ctrl_mode = LIGHT_SPECIFY;
			if (optarg == NULL) {
				fprintf(stderr, "-s NEEDS an argument.\n\n");
				light_print_help();
				return false;
			}

			if (!path_component(optarg)) {
				fprintf(stderr,
					"can't handle controller '%s'\n",
					optarg);
				return false;
			}
			light_conf.ctrl = strdup(optarg);
			break;
			/* -- Value modes -- */
		case 'p':
			ASSERT_VALSET();
			light_conf.val_mode = LIGHT_PERCENT;
			break;
		case 'r':
			ASSERT_VALSET();
			light_conf.val_mode = LIGHT_RAW;
			break;

			/* -- Other -- */
		case 'v':
			if (optarg == NULL) {
				fprintf(stderr, "-v NEEDS an argument.\n\n");
				light_print_help();
				return false;
			}
			if (sscanf(optarg, "%i", &verbosity) != 1) {
				fprintf(stderr,
					"-v Verbosity is not specified in a recognizable format.\n\n");
				light_print_help();
				return false;
			}
			if (verbosity < 0 || verbosity > 3) {
				fprintf(stderr,
					"-v Verbosity has to be between 0 and 3.\n\n");
				light_print_help();
				return false;
			}
			light_loglevel = (light_loglevel_t) verbosity;
			break;
		default:
			light_print_help();
			return false;
		}
	}

	if (!light_check_ops()) {
		light_print_help();
		return false;
	}

	/* If we need a <value> (for writing), make sure we have it! */
	if (light_conf.op_mode == LIGHT_SET ||
	    light_conf.op_mode == LIGHT_ADD ||
	    light_conf.op_mode == LIGHT_SUB) {
		if (argc - optind != 1) {
			LIGHT_ERR("need an argument for <value>");
			light_print_help();
			return false;
		}

		if (light_conf.val_mode == LIGHT_PERCENT) {
			if (sscanf(argv[optind], "%lf", &light_conf.val_pct) != 1) {
				LIGHT_ERR("<value> not specified in a recognizable format");
				light_print_help();
				return false;
			}
			light_conf.val_pct = light_clamp_pct(light_conf.val_pct);
		} else {
			if (sscanf(argv[optind], "%lu", &light_conf.val_raw) != 1) {
				LIGHT_ERR("<value> not specified in a recognizable format");
				light_print_help();
				return false;
			}
		}

	}

	return true;
}
