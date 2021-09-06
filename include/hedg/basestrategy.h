#pragma once
#include "workers/markethandler.h"
#include "market/order.h"
#include "utility/utils.h"
#include "api/messages.h"
#include "spdlog/spdlog.h"

#include <unordered_set>



namespace hedg {

struct Base_Strategy {
	virtual void run_strat() = 0;
	virtual Base_Strategy* clone() = 0;

	void init(io_service& service, ssl::context& context, std::string session_id,
     std::string market_id, std::unique_ptr<Market_Handler>& handler_) {

		client_.reset();
		client_ = utils::new_client(service, context);
		session_id_ = session_id;
		market_id_ = market_id;
		handler = handler_.get();

	}
	std::string get_market_id() {
		return market_id_;
	}
	Base_Strategy* set_country_code(std::unordered_set<std::string> country_codes) {
		country_codes_.insert(country_codes.begin(), country_codes.end());
		return this;
	}
	Base_Strategy* set_event_types(std::unordered_set<std::string> event_types) {
		event_types_.insert(event_types.begin(), event_types.end());
		return this;
	}
	bool is_country_allowed(std::string country_code) {
		if (country_codes_.empty())
			return true;
		if (country_codes_.find(country_code) != country_codes_.end())
			return true;
		else return false;
	}
	bool is_event_allowed(std::string event_type_id) {
		if (event_types_.empty())
			return true;
		if (event_types_.find(event_type_id) != event_types_.end())
			return true;
		else return false;
	}


protected:
	void send_back_order(std::vector<Order>& orders) {
		try {
			client_->send_request(rest::message<params::place_order<Back> >,
				params::place_order<Back>(session_id_, market_id_, orders));
		}
		catch (boost::system::system_error e) {
			spdlog::error(e.what());
			spdlog::warn("Unable to send back order");
		}

	}
	void send_lay_order(std::vector<Order>& orders) {
		try {
			client_->send_request(rest::message<params::place_order<Lay> >,
				params::place_order<Lay>(session_id_, market_id_, orders));
		}
		catch (boost::system::system_error e) {
			spdlog::error(e.what());
			spdlog::warn("Unable to send lay order");
		}
	}
	std::string recv_response() {
		std::string response;
		try {
			response = client_->recv_response();
		}
		catch (boost::system::system_error e) {
			spdlog::error(e.what());
			spdlog::error("Receive response failed");
		}
		return response;
	}

	Market_Handler* handler;

private:
	std::unique_ptr< Client<Rest_Client> >client_;
	std::string session_id_;
	std::string market_id_;
	std::unordered_set<std::string> country_codes_;
	std::unordered_set<std::string> event_types_;

};

}