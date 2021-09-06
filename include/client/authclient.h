#pragma once

#include "client.h"
#include "config/config.h"
#include "api/params.h"
#include "api/messages.h"
#include "simdjson/simdjson.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <ostream>



namespace beast = boost::beast;
namespace http = beast::http;


namespace hedg {

class Client_Auth : public Client<Client_Auth> {

public:

	Client_Auth(const std::string& host_name, const std::string& port_num, io_service& service, ssl::context& ssl_context)
		: Client<Client_Auth>{ host_name, port_num,service, ssl_context }
	{
		sock_.set_verify_mode(ssl::verify_peer);
		sock_.set_verify_callback(std::bind(&Client_Auth::verify_certificate_cb, this, std::placeholders::_1, std::placeholders::_2));
	}

	std::string get_session_id() {
		sock_.handshake(ssl_socket::client);
		http::request<http::string_body> req{ http::verb::post, config::login_endpoint,11 };
		rest::get_session_id_msg(req, params::login());
		http::write(sock_, req);
		http::response<http::string_body> res;
		http::read(sock_, out_buff_, res);
		simdjson::padded_string ln{ std::move(res.body()) };
		simdjson::dom::parser parser;
		std::string_view sess_id;
		auto error = parser.parse(ln)["sessionToken"].get(sess_id);
		return std::string{ sess_id };

	}

	void send() {

	}

private:
	bool verify_certificate_cb(bool preverified, boost::asio::ssl::verify_context& ctx) {
		return true;
	}
};


struct Betfair_Auth {

	static std::string get_session_id() {
		ssl_context.use_certificate_file(config::cert_file, ssl::context::pem);
		ssl_context.use_private_key_file(config::key_file, ssl::context::pem);
		Client_Auth auth{ config::login_host, config::login_port, service, ssl_context };
		return auth.get_session_id();
	}

private:
	Betfair_Auth() {}
	inline static ssl::context ssl_context{ ssl::context::tlsv12_client };
	inline static io_service service;

};

}

