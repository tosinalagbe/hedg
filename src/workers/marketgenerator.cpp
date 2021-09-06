#include "workers/marketgenerator.h"
#include "spdlog/spdlog.h"

namespace hedg {

MarketGenerator::MarketGenerator(int time_span, Timing::time_unit time_unit) : time_span_{ time_span }, time_unit_{ time_unit } {}

void MarketGenerator::generate() {
	
	for (;;) {

		{
			std::scoped_lock<std::mutex> lk(mut_);

			try {
				session_id_ = Betfair_Auth::get_session_id();

				std::string start_time = Timing::now();
				std::string end_time = Timing::after(time_span_, time_unit_);

				io_service service;
				ssl::context context{ ssl::context::tlsv12_client };

				Client<Rest_Client> rest_cl{ config::betting_host, config::betting_port, service, context };
				rest_cl.send_request(rest::message<params::market_catalogue>, params::market_catalogue(session_id_, start_time, end_time));
				simdjson::padded_string p_d{ std::move(rest_cl.recv_response()) };

				simdjson::ondemand::parser parser;

				for (simdjson::ondemand::object market : parser.iterate(p_d)) {
					std::string m_id = std::string(std::string_view(market["marketId"]));
					std::string m_tm = std::string(std::string_view(market["marketStartTime"]));
					std::string event_type = std::string(std::string_view(market["eventType"]["id"]));
					std::string country_code = std::string(std::string_view(market["event"]["countryCode"]));
					markets_.push({ m_id, Market_Projection{m_tm, event_type, country_code} });
				}
			} catch (boost::system::system_error e) {
				spdlog::error(e.what());
			} catch (...) {
				spdlog::error("invalid config info");
				throw std::exception();
			}
		}
		data_cond_.notify_all();
		std::this_thread::sleep_for(std::chrono::seconds(calc_time()));
	}
}

std::queue<std::pair<std::string, Market_Projection> > MarketGenerator::markets() {

	wait_for_gen();
	std::queue<std::pair<std::string, Market_Projection> > temp;
	while (!markets_.empty()) {
		temp.push(markets_.front());
		markets_.pop();
	}
	return temp;
}

void MarketGenerator::poll_within(int time1, int time2) {

	wait_for_gen();
	std::string time_now = Timing::now();
	double diff = Timing::time_diff_mins(markets_.front().second.market_start_time_, time_now);
	if (diff >= time1 && diff <= time2)
		return;
	if (diff < time1) {
		markets_.pop();
	}
	std::this_thread::sleep_for(std::chrono::duration(std::chrono::seconds(10)));
}

void MarketGenerator::wait_for_gen() {
	std::unique_lock<std::mutex> lk(mut_);
	data_cond_.wait(lk, [&] {return !markets_.empty(); });
}



std::queue<std::pair<std::string, Market_Projection> > MarketGenerator::markets_within(int time1, int time2) {

	poll_within(time1, time2);
	std::queue<std::pair<std::string, Market_Projection> > temp;
	while (!markets_.empty()) {
		auto mkt = markets_.front();
		auto time = mkt.second;
		double diff = Timing::time_diff_mins(markets_.front().second.market_start_time_, Timing::now());
		if (diff >= time1 && diff <= time2) {
			temp.push(markets_.front());
			markets_.pop();
		}
		else
			break;

	}
	return temp;
}


std::chrono::seconds MarketGenerator::calc_time() {
	int total_secs = 0;
	if (time_unit_ == Timing::time_unit::hours)
		total_secs += 3600 * time_span_;
	else if (time_unit_ == Timing::time_unit::mins)
		total_secs += 60 * time_span_;
	else
		total_secs += time_span_;
	return std::chrono::seconds(total_secs);
}

}