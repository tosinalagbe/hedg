#pragma once

#include "params.h"

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
using namespace boost::asio;

namespace hedg {
    namespace stream {

	void authenticate_msg(boost::asio::streambuf& request, params::stream_authenticate p);
	void subscribe_msg(boost::asio::streambuf& request, params::stream_subscribe p);

	void order_subscribe_msg(boost::asio::streambuf& request, params::stream_order p);

}

namespace rest {

	namespace {

		template<typename T>
		void set_template(http::request<http::string_body>& req, const T& param) {
			req.set(http::field::host, config::betting_host);
			req.set("X-Application", config::app_key);
			req.set("X-Authentication", param.session_id);
			req.set(http::field::content_type, "application/json");
			req.set(http::field::user_agent, config::user_agent);
			req.set(http::field::accept, "application/json");
		}

	}

	void get_session_id_msg(http::request<http::string_body>& req, params::login login_params);

	void list_market_catalogue_msg(http::request<http::string_body>& req, params::market_catalogue market_params);

	template<typename param_type>
	void message(http::request<http::string_body>& req, param_type param) {
		set_template(req, param);
		req.target() = param.end_point;
		req.body() = param.body();
		req.prepare_payload();
	}

	
	template<typename Bid_Type>
	void place_order_msg(http::request<http::string_body>& req, params::place_order<Bid_Type> order_params) {
		set_template(req, order_params);
		req.target() = order_params.end_point;
		req.body() = order_params.body();
		req.prepare_payload();
	}

}

}

