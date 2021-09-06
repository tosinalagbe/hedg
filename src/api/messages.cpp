#include "api/messages.h"

namespace hedg {

namespace stream {

	void authenticate_msg(boost::asio::streambuf& request, params::stream_authenticate p) {
		std::ostream request_stream(&request);
		request_stream << "{\"op\":\"authentication\",\"id\":1,\"appKey\":\"" << config::app_key << "\","
			"\"session\":\"" << p.session_id << "\"}";
		request_stream << "\r\n";
	}

	void subscribe_msg(boost::asio::streambuf& request, params::stream_subscribe p) {
		std::ostream request_stream(&request);
		request_stream << "{\"op\":\"marketSubscription\",\"id\":\"1\",\"marketFilter\":{\"marketIds\":[" << p.market_ids << "]}"
			",\"marketDataFilter\":{\"fields\":[\"EX_BEST_OFFERS\",\"EX_TRADED\",\"EX_TRADED_VOL\",\"EX_LTP\"]}}";
		request_stream << "\r\n";

	}

	void order_subscribe_msg(boost::asio::streambuf& request, params::stream_order p) {
		std::ostream request_stream(&request);
		request_stream<< "{\"op\":\"orderSubscription\", \"segmentationEnabled\" : true}";
		request_stream << "\r\n";
	}

}

namespace rest {


	void get_session_id_msg(http::request<http::string_body>& req, params::login login_params) {

		req.set(http::field::host, config::login_host);
		req.set("X-Application", config::app_key);
		req.set("X-Authentication", config::user_agent);
		req.set(http::field::content_type, "application/x-www-form-urlencoded");
		req.set(http::field::user_agent, config::user_agent);
		req.body() = login_params.body();
		req.prepare_payload();
	}

	void list_market_catalogue_msg(http::request<http::string_body>& req, params::market_catalogue market_params) {

		set_template(req, market_params);
		req.target() = market_params.end_point;
		req.body() = market_params.body();
		req.prepare_payload();
	}

}

}