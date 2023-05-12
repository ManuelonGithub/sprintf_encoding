# sprintf encoding library
A lightweight C library that will encode the printf format and data onto a buffer instead of processing the format string directly. Relies on a decoder to eventually produce the desired formatted string.   

The idea behind this project is to provide an avenue for logging in systems that either have tight timings of operation, or are constrained in either processing capabilities or instruction space. This is accomplished by deferring printf format processing, to be done by either a different machine or be done at a more convinient time.

MIT License, 
Copyright (c) 2023 ManuelonGithub

## How it works:
This encoding schema will include the printf format text in the output buffer, and will inline the values as raw data alongside a detected format. Because it inlines raw data this means that the output buffer is not a valid c-string, and the mechanisms used to send/receive this data must utilize 8-bit words (or larger) in order to properly transfer all the necessary information.

## How to use it:
The library has been developed so it's fully compatible with the current usage of sprintf. This library is fully thread-safe/reentrant.   
  * `int len = sprintf_df(buf, "the values are %d and %10.4f, the string is \"%s\"\n", 0xdeadbeef, 3.141592, "hello world!");`

The only exception is that it does not support encoding explicit 64-bit values like 'long long' or 'double', and passing them into sprintf_df is considered undefined behaviour.

The data outputted to the buffer is not a valid c-string, and should not be used as such. 

It is also advised that for systems that receive and process user input that may be passed into this library to ideally ensure that the user input text is bound within the standard ASCII range (0-127). 

The library does establish an "interface contract" for decoding data, but it relates to intermediary behaviour that occurs between encoding and decoding. It should be unnecessary to understand if looking to just utilize the library alongside an existing decoder program. 

## How to decode - Interface contract

The encoding schema establishes the following "contract" with a decoder:
  * The character that indicates the start of a printf format is now 0xA5 ('%' + 128). 
    * This was changed so systems that echo/"pass through" user input don't have to perform extra checks to avoid _unintentionally_ triggering a printf format decoding.
    * `"%d"` will be encoded as `\xA5d`, `"%%"` as `\xA5%`, or in other words the decoder should only consider `0xA5` to be the start of a printf format.
  * Encoding will respond and appropriatly inline raw data according the following value specifier char: c,s,%,d,i,u,o,x,X,e,E,f,F,g,G.
  * When inlining string data it will also inline the null-terminator. 
    * This is how the decoder will know that the string data is finished.
    * `("%s\n", "hello world!")` will be encoded as `\xA5shello world!\0\n`
  * The default size for value data (floats, signed/unsigned integers) is 4 bytes, and will inline them from lowest to highest byte. 
    * `("%d\n", 0xdeadbeef)` will be encoded as `\xA5d\xEF\xEE\xAD\xDE\n`
	* The system's endianess is not being accounted for, it'll most likely be the opposite for big-endian systems.
  * the encoder is not required to check the "validity" of the printf format. That is for the decoder to check.
    * Encoder does not need check that `("%1.1.1.1mnkd", 1)` is invalid, and can encode the former as `\xA51.1.1.1mnkd\x01\x00\x00\x00`
    * The decoder must be careful to understand that the encoder will still inline raw data alongside an invalid format, and must still parse out the data accordingly
  * Encoder may utilize a null-terminator to indicate to the decoder that an error was detected and to reset the current decoding state.
    * the invalid format `("%10.4")` will be detected up by the encoder, but it may encode it as `\xA510.4\x00` to defer the error handling to the decoder
    * Decoder must be able to differentiate between a null-terminator being sent in the context of a format error or string data processing.
  * Currently encoding will not inline more than 4 bytes of value data.
    * Chose not to in order to avoid "feature bloat", but can be easily added in the future at the cost of added complexity.
    * `("%lld\n", 1)` will be encoded as `\xA5lld\x01\x00\x00\x00\n`
  * The encoding will inline less raw data if the printf format utilizes short ('h' - 2 bytes of data) or char ('hh' - 1 byte of data) length specifiers.
    * `("%hd\n", 1)` will be encoded as `\xA5hd\x01\x00\n`
    * `("%hhd\n", 1)` will be encoded as `\xA5hhd\x01\n`
  * The encoder will only check and respond to length specifiers directly before the value specifier character
    * `("%llhd\n", 1)` will be encoded as `\xA5llhd\x01\x00\n`
  * Printf wildcards '*' will be followed by a raw 1-byte unsigned value that is meant to replace it.
    * `("%*d\n", 16, 254)` will be encoded as `\xA5*\x10d\xFE\x00\x00\x00\n`
  * As a data saving measure, the decoder must be able to accept a variation of the integer value specifier characters, 128 + <d,i,u,o,x,X>, which signifies that only one unsigned byte of data follows the format. if an input value is within 0 and 255, the encoder can use this functionality to reduce the data being inlined into the output buffer.
    * e.g. `("%d\n", 254)` may be encoded as `\xA5\xE4\xFD\n` instead of `\xA5d\xFD\x00\x00\x00\n`


## For your consideration

This library was designed with simplicity and speed in mind, to do just enough work to provide just enough information for a decoder to do the work of actually formatting a string. Because of that it can be worse in other aspects:
  * It may send more data to a host computer than a fully formatted string. 
    * e.g. `printf("%d\n", 1)` will result in two bytes of text being generated, `"1\n"`, whereas this system will produce 4 bytes of data at best - `\xA5\xE4\x01\n`
  * Because it specifies a very simple byte encoding schema, it is very susceptible to transmission errors which can propagate to corrupt further data when the decoder processes the incoming data.
    * this aspect can be solved by using an error-detection transmission system, or by wrapping data with a parity/checksum byte.

  