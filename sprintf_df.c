
#include <string.h>
#include "sprintf_df.h"

#define DEFAULT_LEN 4	

#define skip_copy(a,b) (*a++ = *b++)
#define skip_ncopy(a, b, n) (a = (char*)memcpy(a, b, (n)) + (n))

int vsprintf_df(char* buf, const char *fmt, va_list args)
{
	char* s;
	int num, len;

	char* str_head;
	char* str = buf;

    while(*fmt) {
		if (*fmt != '%') {
			skip_copy(str, fmt);
			continue;
		}
		
		str_head = str;
		*str_head++ = *fmt++ + 128;	// translate the '%' -> 0xA5
		
		while (1) {
			char c = *fmt;
			skip_copy(str_head, fmt);

			switch (c) {
			case 'c':
				if (str_head == str+2) {
					// data-saving trick - if just %c don't bother with the '%c' bit
					// this can cause problems if the character is 0xA5 though,
					// if that's possible with your system remove this trick
					str_head = str;
				}
				*str_head++ = (unsigned char)va_arg(args, int);
				break;
			case 's':
				s = va_arg(args, char *);
				if (str_head == str+2) {
					// data-saving trick - if just %s don't bother with the '%s' bit
					// this can cause problems if a character in the string is 0xA5 though,
					// if that's possible with your system remove this trick
					str_head = str;
					while (*s) {skip_copy(str_head, s);} // copy until '\0' non-inclusive
				}
				else {
					while (skip_copy(str_head, s) != 0) {} // copy until '\0' inclusive
				}
				break;
			case 'e':
			case 'E':
			case 'f':
			case 'F':
			case 'g':
			case 'G':
				float fnum = (float)va_arg(args, double);
				skip_ncopy(str_head, (char*)&fnum, sizeof(float));
				break;
			case 'd':
			case 'i':
			case 'o':
			case 'x':
			case 'X':
			case 'u':
				len = 4;
				num = va_arg(args, int);
				// determine data length - lengths > 4 bytes not supported to simplify processing
				if (*(fmt-2) == 'h') {
					if (*(fmt-3) == 'h') {
						len = 1;
					}
					else {
						len = 2;
					}
				}

				// data saving trick - for values that fit within 1 unsigned byte
				// force the length to be 1 byte and set a flag in the format specifier
				// that flag being the MSb in the format char (d,i,u,x,X,o)
				if ((unsigned int)num <= 255) {
					*(str_head-1) = c + 128;	// setting flag
					len = 1;
				}
				
				skip_ncopy(str_head, (char*)&num, len);
				break;
			case '%':
				// to save data we can -> str_head = str+1; *str_head++ = c;
				break;
			case '\0':
				fmt--;
				break;
			case '*':
				*str_head++ = (unsigned char)va_arg(args, int);
			default:
				continue;
			}

			str = str_head;
			break;
		}
	}
	return str-buf;
}


int sprintf_df(char* dst, const char* fmt, ...)
{
	va_list args;
    va_start(args, fmt);
    int ret = vsprintf_df(dst, fmt, args);
    va_end(args);

	return ret;
}
