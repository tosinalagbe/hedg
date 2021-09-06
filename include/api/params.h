#pragma once

#include "config/config.h"
#include "market/order.h"

namespace hedg{
namespace params {

	struct market_catalogue {

		std::string session_id;
		std::string start_time;
		std::string end_time;
		std::string end_point;
		std::string event_type_ids;
		std::string market_type_codes;
		std::string max_results;
		std::string market_projection;
		std::string market_countries;
		


		market_catalogue(std::string session_id_, std::string start_time_, std::string end_time_, std::string end_point_ = config::market_catalogue_endpoint,
			std::string event_type_ids_ = config::event_type_ids, std::string market_type_codes_ = config::market_type_codes, 
			std::string market_countries_ = config::market_countries, std::string max_results_ = config::max_results,
			std::string market_projection_ = config::market_projection);

		std::string body();


	};

	template<typename Bid_Type>
	struct place_order {
		std::string end_point;
		std::string session_id;
		std::string market_id;
		std::vector<Order>& orders;

		place_order(std::string session_id_, std::string market_id_, std::vector<Order>&orders_,std::string endpoint_= config::place_order_endpoint);
		std::string body();

	};


	struct login {
		std::string username;
		std::string password;
		std::string endpoint;
		login(std::string username_ = config::username, std::string password_ = config::password, std::string endpoint_ = config::login_endpoint);

		std::string body();

	};

	struct stream_authenticate {
		std::string session_id;
		stream_authenticate(std::string session_id_) : session_id{ session_id_ } {}
	};

	struct stream_subscribe {
		std::string market_ids;
		stream_subscribe(std::string market_ids_) :market_ids{ market_ids_ } {}
	};

	//TODO
	struct stream_order {

	};

	template<typename Bid_Type>
	place_order<Bid_Type>::place_order(std::string session_id_, std::string market_id_,std::vector<Order>&orders_, std::string end_point_)
		:session_id{ session_id_ }, market_id{market_id_}, orders{ orders_ }, end_point{ end_point_ } {}
	
	template<typename Bid_Type>
	std::string place_order<Bid_Type>::body() {
		Bid_Type bid_type;
		return bid_type.instruction(market_id, orders);
	}


}

}




