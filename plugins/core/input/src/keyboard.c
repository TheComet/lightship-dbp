#include "plugin_input/keyboard.h"
#include <stdio.h>

/* ------------------------------------------------------------------------- */
EVENT_LISTENER(on_key_press)
{
	EXTRACT_ARGUMENT(0, key, uint32_t, uint32_t);
}

/* ------------------------------------------------------------------------- */
EVENT_LISTENER(on_key_release)
{
	EXTRACT_ARGUMENT(0, key, uint32_t, uint32_t);
}
