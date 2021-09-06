
#include "timing/timing.h"

#include <chrono>
#include <ctime>
#include <string>

namespace hedg {

std::string Timing::now() {
	std::time_t now = std::time(0);
	tm* gmtm = gmtime(&now);
	std::strftime(buf, time_size, "%FT%TZ", gmtm);
	return buf;
}

std::string Timing::after(size_t duration, time_unit t) {

	std::time_t now = std::time(0);
	switch (t) {
	case secs:
		now += duration;
		break;
	case mins:
		now += (60 * duration);
		break;
	case hours:
		now += (3600 * duration);
	default:
		break;
	}
	tm* gmtm = gmtime(&now);
	std::strftime(buf, time_size, "%FT%TZ", gmtm);
	return buf;
}

//secs
double Timing::time_diff_mins(std::string t1, std::string t2) {
	int year, month, day, hour, min;
	int year2, month2, day2, hour2, min2;
	float sec, sec2;
	sscanf(t1.c_str(), "%d-%d-%dT%d:%d:%fZ", &year, &month, &day, &hour, &min, &sec);
	sscanf(t2.c_str(), "%d-%d-%dT%d:%d:%fZ", &year2, &month2, &day2, &hour2, &min2, &sec2);

	tm time1 = { 0 };
	time1.tm_year = year - 1900;
	time1.tm_mon = month + 1;
	time1.tm_mday = day;
	time1.tm_hour = hour;
	time1.tm_min = min;
	time1.tm_sec = (int)sec;
	time_t first_time = std::mktime(&time1);

	tm time2 = { 0 };
	time2.tm_year = year - 1900;
	time2.tm_mon = month2 + 1;
	time2.tm_mday = day2;
	time2.tm_hour = hour2;
	time2.tm_min = min2;
	time2.tm_sec = (int)sec;
	time_t second_time = std::mktime(&time2);

	return std::difftime(first_time, second_time) / 60;

}

}

