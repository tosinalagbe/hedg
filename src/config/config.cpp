#include "config/config.h"

namespace hedg {

namespace config {

	//no need to change these
	std::string login_host = "identitysso-cert.betfair.com";
	std::string login_port = "443";
	std::string betting_host = "api.betfair.com";
	std::string betting_port = "443";
	std::string stream_host = "stream-api.betfair.com";
	std::string stream_port = "443";
	std::string market_type_codes = R"("WIN")";
	std::string in_play_only = "false";
	std::string max_results = R"("300")";
	std::string market_data_filter = "";
	std::string market_projection = R"("MARKET_START_TIME","EVENT_TYPE","EVENT")";
	std::string market_catalogue_endpoint = "/exchange/betting/rest/v1.0/listMarketCatalogue/";
	std::string login_endpoint = "/api/certlogin";
	std::string place_order_endpoint = "/exchange/betting/json-rpc/v1/";
	std::string user_agent = "hedg";


	//user config
	std::string username = "useremail@domain.com";
	std::string password = "userpassword$";
	std::string app_key = "betfairappkey";
	std::string key_file = "path-to-.key-file";
	std::string cert_file = "path-to-.crt-file";


	//framework config
	//default of 7 mins per market, edit as convenient.
	int trade_span = 7;

	//market config
	//note that you can still select a subset of these for your strategies as shown in the example
	std::string event_type_ids = R"("4339","7")";
	std::string market_countries = R"("AU","US","GB","IE","NZ","ZA","FR")";

	//change with respect to pc hardware capability to minimize latency and increase throughput
	//max number of markets that can run in parallel
	int max_markets=3;
};

}
