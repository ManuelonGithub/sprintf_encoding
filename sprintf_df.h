/**
 * @file 	sprintf_df.h
 * @author 	ManuelonGithub (manuelonemail@gmail.com)
 * @brief 	Simple C library that will encode printf format and data into a buffer 
 * 			so its processing can be deferred
 * 
 * @copyright Copyright (c) 2023 ManuelonGithub
 * 
 * MIT License
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef SPRINTF_DF_H_
#define SPRINTF_DF_H_

#include <stdarg.h>

/**
 * @brief deferred sprintf - va_list edition!
 * @param	dst byte buffer to insert processed data into - output is not a c-string!
 * @param	fmt printf format string
 * @param	args variable list of arguments to be processed according to the format
 * @return	length of the content in dst
 * @note	This function is fully thread-safe/reetrant.
 * @warning	dst will not be c-string compatible. 
 */
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