/**
 * @file
 *
 * @date 18.10.09
 * @author Nikolay Korotky
 */
#include <string.h>

typedef unsigned chartype;

char *strstr (const char *phaystack, const char *pneedle) {
	const unsigned char *haystack, *needle;
	unsigned b;
	const unsigned char *rneedle;

	haystack = (const unsigned char *) phaystack;

	if ((b = *(needle = (const unsigned char *) pneedle))) {
		chartype c;
		haystack--; /* possible ANSI violation */

		{
			unsigned a;
			do
				if (!(a = *++haystack))
					goto ret0;
			while (a != b);
		}

		if (!(c = *++needle))
			goto foundneedle;
		++needle;
		goto jin;

		for (;;) {
			{
				unsigned a;
				if (0)
				jin:{
					if ((a = *++haystack) == c)
						goto crest;
				} else
					a = *++haystack;
				do {
					for (; a != b; a = *++haystack) {
						if (!a)
							goto ret0;
						if ((a = *++haystack) == b)
							break;
						if (!a)
							goto ret0;
					}
				} while ((a = *++haystack) != c);
			}
			crest:
			{
				unsigned a;
				{
					const unsigned char *rhaystack;
					if (*(rhaystack = haystack-- + 1) == (a = *(rneedle = needle)))
						do {
							if (!a)
								goto foundneedle;
							if (*++rhaystack != (a = *++needle))
								break;
							if (!a)
								goto foundneedle;
						} while (*++rhaystack == (a = *++needle));
					needle = rneedle;	/* took the register-poor aproach */
				}
				if (!a)
					break;
			}
		}
	}
foundneedle:
	return (char *) haystack;
ret0:
	return 0;
}
