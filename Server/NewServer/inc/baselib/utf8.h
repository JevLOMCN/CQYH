/*
 * Copyright (c) 2007 Alexey Vatchenko <av@bsdua.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * utf8: implementation of UTF-8 charset encoding (RFC3629).
 */
#ifndef _UTF8_H_
#define _UTF8_H_

#include <sys/types.h>

#include <wchar.h>

#define UTF8_IGNORE_ERROR		0x01
#define UTF8_SKIP_BOM			0x02
#define UTF32_BOM               (0x0000FEFF)
#define UTF16_BOM               (0xFEFF)
#define UTF8_BOM                (0xBFBBEF)

size_t		utf8_to_wchar(const char *in, size_t insize, wchar_t *out,
		    size_t outsize, int flags);
size_t		wchar_to_utf8(const wchar_t *in, size_t insize, char *out,
		    size_t outsize, int flags);

// Windows code-page conversions. Results are caller-owned new[] arrays; release
// with free_converted_string or adopt into unique_ptr<char[]>. Failure returns null.
char* char_to_utf8(char* str_mb);            // Convert an ANSI char string to UTF-8
char* utf8_to_char(const char* str_utf8);    // Convert a UTF-8 char string to ANSI
void free_converted_string(char* str);       // Helper function: free memory

inline size_t utf8_2_utf16(const char* pcszSrc, size_t uSrcLen, wchar_t* pszDest, size_t uDestLen)
{
    return utf8_to_wchar(pcszSrc, uSrcLen, pszDest, uDestLen, UTF8_IGNORE_ERROR | UTF8_SKIP_BOM);
}

inline size_t utf16_2_utf8(const wchar_t* pcszSrc, size_t uSrcLen, char* pszDest, size_t uDestLen)
{
    return wchar_to_utf8(pcszSrc, uSrcLen, pszDest, uDestLen, UTF8_IGNORE_ERROR | UTF8_SKIP_BOM);
}

#endif /* !_UTF8_H_ */
