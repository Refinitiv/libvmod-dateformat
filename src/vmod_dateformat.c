/*
 * Copyright 2017 Thomson Reuters
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#define _XOPEN_SOURCE /* glibc2 needs this */
#include <time.h>

/* need vcl.h before vrt.h for vmod_evet_f typedef */
#include "vcl.h"
#include "vrt.h"
#include "cache/cache.h"

#include "vtim.h"
#include "vcc_dateformat_if.h"

static const int days_before_month[] = {
	0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
};

/* largely inspired by Varnish lib vtim.c */
double
utc_mktime(const struct tm *tm)
{
	double t;
	int month = 0, year = 0, mday = 0;
	int hour = 0, min = 0, sec = 0;
	int d, leap;

	month = tm->tm_mon + 1;
	year = tm->tm_year + 1900;
	mday = tm->tm_mday;
	hour = tm->tm_hour;
	min = tm->tm_min;
	sec = tm->tm_sec;
	leap =
	    ((year) % 4) == 0 && (((year) % 100) != 0 || ((year) % 400) == 0);
	t = ((hour * 60.) + min) * 60. + sec;
	d = (mday - 1) + days_before_month[month - 1];

	if (month > 2 && leap)
		d++;

	d += (year % 100) * 365;	/* There are 365 days in a year */

	if ((year % 100) > 0)		/* And a leap day every four years */
		d += (((year % 100) - 1) / 4);

	d += ((year / 100) - 20) *	/* Days relative to y2000 */
	    (100 * 365 + 24);		/* 24 leapdays per year in a century */

	d += ((year - 1) / 400) - 4;	/* And one more every 400 years */

	t += d * 86400.;

	t += 10957. * 86400.;		/* 10957 days frm UNIX epoch to y2000 */

	return (t);
}

VCL_STRING __match_proto__(td_dateformat_time2date)
vmod_time2date(VRT_CTX, VCL_STRING format, VCL_TIME time)
{
	struct tm tm;
	time_t tt;
	size_t max, size;
	char *p;

	if (format == NULL) {
		VSLb(ctx->vsl, SLT_Error, "date2str: bad arguments");
		return "";
	}

        tt = (time_t)time;
        (void)gmtime_r(&tt, &tm);

	max = WS_Reserve(ctx->ws, 0);
	if (max == 0) {
		VSLb(ctx->vsl, SLT_Error, "date2str: workspace is empty");
		return "";
	}
	p = ctx->ws->f;
	size = strftime(p, max, format, &tm);
	if (size == 0) {
		WS_ReleaseP(ctx->ws, p);
		VSLb(ctx->vsl, SLT_Error, "strftime: <%ld> in <%s> format doesn't fit in <%ld> bytes", tt, format, max);
		return NULL;
	}
	WS_Release(ctx->ws, size + 1);
	return p;
}

VCL_TIME __match_proto__(td_dateformat_date2time)
vmod_date2time(VRT_CTX, VCL_STRING format, VCL_STRING str, VCL_TIME fallback)
{
	time_t time;
	struct tm tm;

	if (format == NULL || str == NULL) {
		VSLb(ctx->vsl, SLT_Error, "str2date: bad arguments");
		return fallback;
	}

	if (strptime(str, format, &tm) == NULL) {
		VSLb(ctx->vsl, SLT_Error, "strptime failed: <%s> is not in <%s> format", str, format);
		return fallback;
	}
	time = utc_mktime(&tm);
	if (time == -1) {
		VSLb(ctx->vsl, SLT_Error, "mktime failed: <%s> is not in <%s> format", str, format);
		return fallback;
	}
	return time;
}
