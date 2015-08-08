#include "framework/se_api.h"
#include "util/ptree.h"

/* ------------------------------------------------------------------------- */
char
directory_name_is_valid(const char* directory)
{
	const char* p;
	char c;
	for(p = directory; (c = *p); ++p)
	{
		if((c < 48 || c > 57)  &&   /* 0-9 */
		   (c < 65 || c > 90)  &&   /* A-Z */
		   (c < 97 || c > 122) &&   /* a-z */
		   (c != '_')          &&   /* underscore */
		   (c != ptree_node_delim)) /* namespace delimiters */
		{
			return 0;
		}

	}

	return 1;
}
