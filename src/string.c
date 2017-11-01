/*
 * Copyright 2017 Patrick O. Perry.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <assert.h>
#include "rutf8.h"

static void bytes_init(struct rutf8_bytes *bytes, SEXP charsxp);
static int text_init(struct utf8lite_text *text, SEXP charsxp);


void rutf8_string_init(struct rutf8_string *str, SEXP charsxp)
{
	if (charsxp == NA_STRING) {
		str->type = RUTF8_STRING_NONE;
	} else if (!text_init(&str->value.text, charsxp)) {
		str->type = RUTF8_STRING_TEXT;
	} else {
		bytes_init(&str->value.bytes, charsxp);
		str->type = RUTF8_STRING_BYTES;
	}
}


void bytes_init(struct rutf8_bytes *bytes, SEXP charsxp)
{
	assert(charsxp != NA_STRING);
	bytes->ptr = (const uint8_t *)CHAR(charsxp);
	bytes->size = (size_t)XLENGTH(charsxp);
}


int text_init(struct utf8lite_text *text, SEXP charsxp)
{
	const uint8_t *ptr;
	size_t size;
	cetype_t ce;
	int err = 0;

	assert(charsxp != NA_STRING);

	ce = getCharCE(charsxp);
	if (encodes_utf8(ce)) {
		ptr = (const uint8_t *)CHAR(charsxp);
		size = (size_t)XLENGTH(charsxp);
	} else if (ce == CE_LATIN1 || ce == CE_NATIVE) {
		ptr = (const uint8_t *)translate_utf8(charsxp);
		size = strlen((const char *)ptr);
	} else {
		err = UTF8LITE_ERROR_INVAL; // bytes or other encoding
		goto exit;
	}

	TRY(utf8lite_text_assign(text, ptr, size, 0, NULL));
exit:
	return err;
}


int rutf8_string_width(const struct rutf8_string *str, int flags)
{
	switch (str->type) {
	case RUTF8_STRING_TEXT:
		return rutf8_text_width(&str->value.text, flags);
	case RUTF8_STRING_BYTES:
		return rutf8_bytes_width(&str->value.bytes, flags);
	default:
		return -1;
	}
}


int rutf8_string_lwidth(const struct rutf8_string *str, int flags,
			int limit, int ellipsis)
{
	switch (str->type) {
	case RUTF8_STRING_TEXT:
		return rutf8_text_lwidth(&str->value.text, flags, limit,
					 ellipsis);
	case RUTF8_STRING_BYTES:
		return rutf8_bytes_lwidth(&str->value.bytes, flags, limit,
					  ellipsis);
	default:
		return -1;
	}
}


int rutf8_string_rwidth(const struct rutf8_string *str, int flags,
			int limit, int ellipsis)
{
	switch (str->type) {
	case RUTF8_STRING_TEXT:
		return rutf8_text_rwidth(&str->value.text, flags,
					 limit, ellipsis);
	case RUTF8_STRING_BYTES:
		return rutf8_bytes_rwidth(&str->value.bytes, flags,
					  limit, ellipsis);
	default:
		return -1;
	}
}


SEXP rutf8_string_lformat(struct utf8lite_render *r,
			  const struct rutf8_string *str,
			  int trim, int chars, int width_max,
			  int quote, int utf8, int flags, int centre)
{
	switch (str->type) {
	case RUTF8_STRING_TEXT:
		return rutf8_text_lformat(r, &str->value.text, trim, chars,
					  width_max, quote, utf8, flags,
					  centre);
	case RUTF8_STRING_BYTES:
		return rutf8_bytes_lformat(r, &str->value.bytes, trim, chars,
					   width_max, quote, utf8, flags,
					   centre);
	default:
		return NA_STRING;
	}
}


SEXP rutf8_string_rformat(struct utf8lite_render *r,
			  const struct rutf8_string *str,
			  int trim, int chars, int width_max,
			  int quote, int utf8, int flags)
{
	switch (str->type) {
	case RUTF8_STRING_TEXT:
		return rutf8_text_rformat(r, &str->value.text, trim, chars,
					  width_max, quote, utf8, flags);
	case RUTF8_STRING_BYTES:
		return rutf8_bytes_rformat(r, &str->value.bytes, trim, chars,
					   width_max, quote, utf8, flags);
	default:
		return NA_STRING;
	}
}