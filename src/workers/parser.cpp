#include "workers/parser.h"
#include <optional>


namespace hedg {

Parser::Parser(std::unique_ptr<Market_Handler>& handler, int max_markets)
	:handler_ {handler}
{
	for (int i = 0; i < max_markets; ++i) {
		full_jsons.push_back("");
		full_jsons[full_jsons.size() - 1].reserve(2000);
		
	}
	valid_parse = false;
}

Parser& Parser::parse(int i, const std::string& json) {

	valid_parse = false;
	for (auto it = json.begin(); it != json.end(); ++it) {
		
		full_jsons[i] += *it;
 		if (*it == '\n') {
			doc_ = parser_.parse(simdjson::padded_string_view(full_jsons[i].c_str(),
				full_jsons[i].length(), full_jsons[i].length() + simdjson::SIMDJSON_PADDING));
			valid_parse = true;
			full_jsons[i].clear();
		}
	}	
	return *this;

}



void Parser::parse_and_update(const std::deque<std::pair<std::string, Market_Projection>>& subscribed) {
	
	if (!valid_parse)
		return;
	auto error = doc_["mc"].get(market_array);
	if (error)
		return;

	auto img_error = doc_["ct"].get(img);
	for (auto mkts : market_array) {
		std::string_view id;
		
		mkts["id"].get(id);
		std::string market_id = std::string(id);
		for (auto runners : mkts["rc"]) {
			int runner_id = runners["id"].get_int64();
			
			if (!img_error && img == "SUB_IMAGE") {
				handler_->add_runner(market_id, runner_id);
			}

			parse_trd(market_id, runner_id, runners);
			parse_batb(market_id, runner_id, runners);
			parse_batl(market_id, runner_id, runners);
			
		}
		auto tv_error = mkts["tv"].get(total_traded_volume);
		if (!tv_error)
			handler_->update_total_traded_volume(market_id, total_traded_volume.get_double());
		
		if (!img_error && img == "SUB_IMAGE") {
			for (auto mkt : subscribed) {
				if (market_id == mkt.first)
				{
					handler_->activate_market(market_id, mkt.second);
				}
			}
			//handler_->activate_market(market_id);
		}
			


	}
}

void Parser::reset(int i){

	if (full_jsons[i].empty())
		return;
	full_jsons[i].clear();
	valid_parse = false;
}

void Parser::parse_batb(std::string& market_id, int runner_id, simdjson::dom::element& runners){

	auto batb_error = false;
	batb_error = runners["batb"].get(batb_array);
	if (!batb_error) {
		for (auto prices_level : batb_array) {
			int idx = 0;
			int lvl = 0;
			float price = 0, size = 0;
			for (auto prices : prices_level) {
				if (idx == 0)
					lvl = prices.get_int64();
				else if (idx == 1)
					price = prices.get_double();
				else if (idx == 2)
					size = prices.get_double();
				idx += 1;
			}
			Bid b1{ price,size };
			handler_->update_back(market_id, runner_id, lvl, b1);
		}
	}

}

void Parser::parse_batl(std::string& market_id, int runner_id, simdjson::dom::element& runners){
	auto batl_error = false;
	batl_error = runners["batl"].get(batl_array);
	if (!batl_error) {
		for (auto prices_level : batl_array) {
			int idx = 0;
			int lvl;
			float price = 0, size = 0;
			//std::cout << "batb: ";
			for (auto prices : prices_level) {
				//std::cout << prices.get_double() << " ";
				if (idx == 0)
					lvl = prices.get_int64();
				else if (idx == 1)
					price = prices.get_double();
				else if (idx == 2)
					size = prices.get_double();

				idx += 1;
			}
			Bid b2{ price,size };
			handler_->update_lay(market_id, runner_id, lvl, b2);

		}
	}

}


void Parser::parse_trd(std::string& market_id, int runner_id, simdjson::dom::element& runners) {
	

	auto traded_error = false; //
	traded_error = runners["trd"].get(traded_array);
	if (!traded_error) {
		std::optional<float> traded_vol = std::nullopt;
		
		auto traded_vol_error = runners["tv"].get(traded_vol_);
		if (!traded_vol_error)
			traded_vol = traded_vol_.get_double();
		
		std::optional<float> ltp = std::nullopt;
		auto ltp_error = runners["ltp"].get(ltp_);
		if (!ltp_error)
			ltp = ltp_.get_double();
		
		for (auto traded : traded_array) {
			int idx = 0;
			float price = 0, size = 0;
			for (auto prices : traded) {
				if (idx == 0)
					price = prices.get_double();
				if (idx == 1)
					size = prices.get_double();
				idx += 1;
			}
			Bid b1{ price, size };
			handler_->update_traded(market_id, runner_id, ltp, traded_vol, b1);
		}
	}
}

}
