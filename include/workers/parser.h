#pragma once

#include "markethandler.h"
#include "utility/queue.h"
#include "simdjson/simdjson.h"
#include <boost/tokenizer.hpp>
#include "config/config.h"
#include "market/marketprojection.h"

namespace hedg{

class Parser {

	simdjson::dom::parser parser_;
	simdjson::dom::element doc_;

	std::unique_ptr<Market_Handler>& handler_;
	std::vector<std::string> full_jsons;
	bool valid_parse;

	simdjson::dom::array market_array;
	simdjson::dom::array traded_array;
	simdjson::dom::array batb_array;
	simdjson::dom::array batl_array;
	simdjson::dom::element total_traded_volume;
	simdjson::dom::element ltp_;
	simdjson::dom::element traded_vol_;
	std::string_view img;

	void parse_batb(std::string& market_id, int runner_id, simdjson::dom::element& runners);
	void parse_batl(std::string& market_id, int runner_id, simdjson::dom::element& runners);
	void parse_trd(std::string& market_id, int runner_id, simdjson::dom::element& elem);
	
public:
	Parser(std::unique_ptr<Market_Handler>& handler_, int max_markets= config::max_markets);
	Parser& parse(int i, const std::string& json);
	void parse_and_update(const std::deque<std::pair<std::string, Market_Projection>>& subscribed);
	void reset(int i);
	
};

}