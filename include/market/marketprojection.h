#pragma once
#include <string>

namespace hedg {

struct Market_Projection {
	Market_Projection() {}
	Market_Projection(std::string market_start_time, std::string event_type, std::string country_code) :
		market_start_time_{ market_start_time }, event_type_{ event_type }, country_code_{ country_code }{}
	std::string market_start_time_;
	std::string event_type_;
	std::string country_code_;
};

}
