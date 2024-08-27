#include "include/date.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

const char *DAY[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
const char *MONTH[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

struct tm *get_gmtime()
{
	time_t t = time(NULL);
	return gmtime(&t);
}

char *generate_date_string()
{
	char *res = calloc(30, sizeof(char));
	struct tm time = *get_gmtime();
	snprintf(res, 30, "%s, %02d %s %04d %02d:%02d:%02d %s", DAY[time.tm_wday], time.tm_mday, MONTH[time.tm_mon], time.tm_year + 1900, time.tm_hour, time.tm_min, time.tm_sec, "GMT");
	return res;
}