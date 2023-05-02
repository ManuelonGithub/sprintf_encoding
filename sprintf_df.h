

#ifndef SPRINTF_DF_H_
#define SPRINTF_DF_H_

#include <stdarg.h>

int vsprintf_df(char* buf, const char *fmt, va_list args);

/**
 * @brief deferred sprintf
 * @param	dst byte buffer to insert processed data into - output is not a c-string!
 * @param	fmt printf format string
 * @param	... variable arguments to be processed according to the format
 * @return	length of the content in dst
 * @note	This function is fully thread-safe/reetrant.
 * @warning	dst will not be c-string compatible. 
 */
__attribute__ ((format (printf, 2, 3))) int sprintf_df(char* dst, const char* fmt, ...);

#endif // !SPRINTF_DF_H_