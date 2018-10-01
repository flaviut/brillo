#include <math.h>

#include "value.h"
#include "helpers.h"

/**
 * value_clamp:
 * @val:	value to clamp
 * @min:	minimum value to use
 * @max:	maximum value to use
 *
 * Returns: clamped value
 **/
uint64_t value_clamp(uint64_t val, uint64_t min, uint64_t max)
{
	if (val < min) {
		LIGHT_WARN("Raising value '%" SCNu64 "' to '%" SCNu64 "'", val, min);
		return min;
	} else if (val > max) {
		LIGHT_WARN("Lowering value '%" SCNu64 "' to '%" SCNu64 "'", val, max);
		return max;
	} else {
		return val;
	}
}

/**
 * value_from_raw:
 * @mode:	mode used to calculate value
 * @raw:	raw value to use in calculation
 * @max:	raw maximum value to use
 *
 * Calculates a linear or exponential percentage.
 *
 * Returns: the percentage, or a negative value on error
 **/
uint64_t value_from_raw(LIGHT_VAL_MODE mode, uint64_t raw, uint64_t max)
{
	if (mode == LIGHT_RAW)
		return raw;
	else if (mode == LIGHT_PERCENT)
		return VALUE_CLAMP_PCT((raw * VALUE_PCT_MAX) / max);
	else if (mode == LIGHT_PERCENT_EXPONENTIAL)
		return (uint64_t) ((log((double) raw) / log((double) max)) * VALUE_PCT_MAX);
	else
		return -1;
}

/**
 * value_to_raw:
 * @mode:	value mode used to calculate raw value
 * @val:	value to convert to raw value
 * @max:	raw maximum value to use
 *
 * Calculates a raw value based on the value mode.
 *
 * Returns: the raw value
 **/
uint64_t value_to_raw(LIGHT_VAL_MODE mode, uint64_t val, uint64_t max)
{
	if (mode == LIGHT_RAW)
		return val;
	else if (mode == LIGHT_PERCENT)
		return ((val * max) / VALUE_PCT_MAX);
	else if (mode == LIGHT_PERCENT_EXPONENTIAL)
		return (uint64_t) (exp((double) val * log((double) max) / VALUE_PCT_MAX));
	else
		return -1;
}
