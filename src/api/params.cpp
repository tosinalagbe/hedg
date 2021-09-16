#include "config/config.h"
#include "api/params.h"

#include "fmt/core.h"


namespace hedg{

namespace params {

	market_catalogue::market_catalogue(std::string session_id_, std::string start_time_, std::string end_time_, std::string end_point_, std::string event_type_ids_,
		std::string market_type_codes_, std::string market_countries_ , std::string max_results_ ,std::string market_projection_)
		:session_id{ session_id_ }, start_time{ start_time_ }, end_time{ end_time_ }, end_point{end_point_}, event_type_ids{ event_type_ids_ },
		market_type_codes{ market_type_codes_ }, market_countries{ market_countries_ }, max_results{ max_results_ },
		market_projection{ market_projection_ } {}

	std::string market_catalogue::body() {

		std::string body;

		fmt::format_to(std::back_inserter(body),"{{"
			"\"filter\":{{"
			"\"eventTypeIds\" : [{}] ,"
			"\"marketCountries\" : [{}] ,"
			"\"marketTypeCodes\" : [{}] ,"
			"\"inPlayOnly\" : \"false\","

			"\"marketStartTime\" : {{"

			"\"from\": \"{}\","
			"\"to\" : \"{}\""
			"}}"

			"}},"
			"\"sort\": \"FIRST_TO_START\","
			"\"maxResults\" : \"100\","
			"\"marketProjection\" : ["

			"{}"

			"]"
			"}}", event_type_ids, market_countries, market_type_codes, start_time, end_time, market_projection);

		return body;

	}

	
	login::login(std::string username_, std::string password_, std::string endpoint_)
		:username{ username_ }, password{ password_ }, endpoint{ endpoint_ } {}

	std::string login::body() {

		std::string body;
		fmt::format_to(std::back_inserter(body),"username={}&password={}", username, password);
		return body;
	}

}

}

