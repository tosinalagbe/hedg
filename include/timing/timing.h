#pragma once
#include <chrono>
#include <ctime>
#include <string>


namespace hedg {
	class Timing {

	public:
		enum time_unit { secs, mins, hours };

		static std::string now();
		static std::string after(size_t duration, time_unit t = mins);

		//secs
		static double time_diff_mins(std::string t1, std::string t2);

	private:
		inline static const size_t time_size = sizeof "2011-10-08T07:07:09Z";
		inline static char buf[time_size];

	};
}

