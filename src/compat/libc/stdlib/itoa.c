/**
 * @file
 * @brief Convert integer to string.
 *
 * @see stdlib.h
 *
 * @date 01.04.13
 * @author Károly Oláh
 *          - Initial commit
 */

char *itoa( int num, char *buf, unsigned short int base )
{
	char *p = buf;
	char *p1, *p2;
	int ud = 0;

	*buf = '\0';	/* initialize buffer. In the case of an error, this will already be in the buffer, indicating that the result is invalid (NULL). */
	p1 = buf;	/* start of buffer */

	// check base
	if( base < 2 || base > 36 )
		{ return buf; }

	/* If num < 0, put `-' in the head.  */
	if( num < 0 )
	{
		*(p++) = '-';
		p1++;
		ud = -num;
	}
	else
		{ ud = num; }

	/* Divide ud by base until ud == 0.  */
	int remainder = 0;
	do {
		remainder = ud % base;
		*(p++) = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
	} while( ud /= base );

	/* Terminate buf.  */
	*p = '\0';

	/* Reverse buffer.  */
	p2 = p - 1;	/* end of buffer */
	char tmp;
	while( p1 < p2 )
	{
		tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}
	return buf;
}
