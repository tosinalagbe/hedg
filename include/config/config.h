#pragma once
#include <string>

namespace hedg{

namespace config {

	//no need to change these
	extern std::string login_host;
	extern std::string login_port;
	extern std::string betting_host;
	extern std::string betting_port;
	extern std::string stream_host;
	extern std::string stream_port;
	//raw string literal
	extern std::string market_type_codes;
	extern std::string in_play_only;
	//raw string literal
	extern std::string max_results;
	extern std::string market_data_filter;
	//raw string literal
	extern std::string market_projection;
	extern std::string market_catalogue_endpoint;
	extern std::string login_endpoint;
	extern std::string place_order_endpoint;
	extern std::string user_agent;

	//user config
	extern std::string username;
	extern std::string password;
	extern std::string app_key;	
	extern std::string key_file;
	extern std::string cert_file;

	//framework config
	//time in mins to trade on market
	extern int trade_span;
	
	//market config
	//raw string literal
	extern std::string event_type_ids;
	//raw string literal
	extern std::string market_countries;

	//change with respect to pc hardware capability to minimize latency and increase throughput
	//max number of markets that can run in parallel
	extern int max_markets;

	//optional
	//to be implemented by user
	void load_config(std::string config_filename);
};

}